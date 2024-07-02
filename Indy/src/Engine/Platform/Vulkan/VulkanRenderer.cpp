#include <Engine/Core/LogMacros.h>

#include <memory>
#include <vector>

#include <chrono>
#include <iostream>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

import Indy.Graphics;
import Indy.VulkanGraphics;
import Indy.Window;

import Indy.Profiler;

namespace Indy
{
	VulkanRenderer::VulkanRenderer(Window* window, const VkInstance& instance, const std::shared_ptr<VulkanDevice>& device)
		: m_Window(window), m_Instance(instance), m_Device(device), m_CurrentFrameIndex(0)
	{
		// Generate a surface for the window
		if (glfwCreateWindowSurface(m_Instance, static_cast<GLFWwindow*>(m_Window->NativeWindow()), nullptr, &m_Surface) != VK_SUCCESS)
		{
			INDY_CORE_CRITICAL("Failed to create window surface!");
			return;
		}

		// Ensure the logical device can properly support swapchain presentation
		if (!QueryVulkanSwapchainSupport(m_Device, m_Surface))
		{
			INDY_CORE_ERROR("Failed to initialize Renderer for window [{0}]: Presentation is not supported!", m_Window->Properties().title);
			return;
		}

		m_Swapchain = VulkanSwapchain::Create(m_Window, m_Surface, m_Device);

		// Retrieve Vulkan Queue Handles
		{
			VkDeviceQueueInfo2 queueGetInfo{};
			queueGetInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
			queueGetInfo.pNext = nullptr;
			queueGetInfo.flags = 0;
			queueGetInfo.queueIndex = 0;

			// Retrieve Compute Queue
			queueGetInfo.queueFamilyIndex = m_Device->queueFamilies.compute.value();
			vkGetDeviceQueue2(m_Device->handle, &queueGetInfo, &m_ComputeQueue);

			// Retrieve Graphics Queue
			queueGetInfo.queueFamilyIndex = m_Device->queueFamilies.graphics.value();
			vkGetDeviceQueue2(m_Device->handle, &queueGetInfo, &m_GraphicsQueue);

			// Retrieve Present Queue (NOTE: This can be the same as the graphics queue)
			queueGetInfo.queueFamilyIndex = m_Device->queueFamilies.present.value();
			vkGetDeviceQueue2(m_Device->handle, &queueGetInfo, &m_PresentQueue);
		}

		// Allocate resources for the maximum number of 'in-flight' frames
		m_Frames.resize(g_Max_Frames_In_Flight);
		for (auto& frame : m_Frames)
			AllocateVulkanFrameData(m_Device->handle, m_Device->queueFamilies, frame);

		// Setup Immediate Commands
		{
			// Initialize immediate command pool with the graphics queue
			m_ImmediateCmdPool.Allocate(m_Device->handle, m_Device->queueFamilies.graphics.value(), 1);

			// Create the fence for the immediate command pool
			VkFenceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			// Immediate Fence
			if (vkCreateFence(m_Device->handle, &createInfo, nullptr, &m_ImmediateFence) != VK_SUCCESS)
			{
				INDY_CORE_ERROR("Failed to create sync object [VkFence]!");
				return;
			}
		}

		// Create the image allocator
		{
			VmaAllocatorCreateInfo allocatorInfo{};
			allocatorInfo.physicalDevice = m_Device->physicalDevice;
			allocatorInfo.device = m_Device->handle;
			allocatorInfo.instance = m_Instance;
			allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT; // or other flags

			if (vmaCreateAllocator(&allocatorInfo, &m_ImageAllocator) != VK_SUCCESS)
			{
				INDY_CORE_ERROR("Failed to initialize Vulkan Renderer: Image Allocation Failed!");
				return;
			}
		}

		// Create Render Image (NOTE: Likely needs to happen again if the window resizes)
		{
			// Create the image to render to
			VulkanImageSpec imageSpec{};
			imageSpec.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageSpec.extent.width = m_Window->Properties().width;
			imageSpec.extent.height = m_Window->Properties().height;
			imageSpec.extent.depth = 1;

			// Perhaps not suitable for ALL operations, but will match the selected swapchain format
			imageSpec.format = VK_FORMAT_B8G8R8A8_UNORM;

			imageSpec.usageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			imageSpec.usageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			imageSpec.usageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
			imageSpec.usageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			imageSpec.usageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;

			imageSpec.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;

			// Create Image
			m_RenderImage = VulkanImage::Create(m_Device->handle, m_ImageAllocator, imageSpec);
		}

		// Build Render Pipelines
		BuildPipelines();

		// Initialize ImGUI for this renderer
		InitImGui();

		// Create the image descriptor for ImGui's backend
		m_ImGuiRenderImageDescriptor = ImGui_ImplVulkan_AddTexture(m_RenderImage.sampler, m_RenderImage.view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	VulkanRenderer::~VulkanRenderer()
	{
		// Wait for graphics operations to finish
		vkDeviceWaitIdle(m_Device->handle);

		// Shutdown ImGui
		{
			ImGui_ImplVulkan_RemoveTexture(m_ImGuiRenderImageDescriptor);
			ImGui_ImplVulkan_Shutdown();
			vkDestroyDescriptorPool(m_Device->handle, m_ImGuiDescriptorPool, nullptr);
		}

		// Cleanup Immediate submission resources
		{
			vkDestroyFence(m_Device->handle, m_ImmediateFence, nullptr);
		}

		// Cleanup Frames
		{
			for (const auto& frame : m_Frames)
			{
				vkDestroySemaphore(m_Device->handle, frame.computeSemaphore, nullptr);
				vkDestroySemaphore(m_Device->handle, frame.graphicsSemaphore, nullptr);
				vkDestroySemaphore(m_Device->handle, frame.swapchainSemaphore, nullptr);
				vkDestroyFence(m_Device->handle, frame.computeFence, nullptr);
				vkDestroyFence(m_Device->handle, frame.graphicsFence, nullptr);
			}

			// Clear the vector to trigger command pool destructino
			m_Frames.clear();
		}

		// Cleanup Render Image
		{
			vkDestroySampler(m_Device->handle, m_RenderImage.sampler, nullptr);
			vkDestroyImageView(m_Device->handle, m_RenderImage.view, nullptr);
			vmaDestroyImage(m_ImageAllocator, m_RenderImage.image, m_RenderImage.allocation);
			vmaDestroyAllocator(m_ImageAllocator);
		}

		// Cleanup Swapchain
		{
			for (auto& view : m_Swapchain.imageViews)
			{
				vkDestroyImageView(m_Device->handle, view, nullptr);
			}

			vkDestroySwapchainKHR(m_Device->handle, m_Swapchain.handle, nullptr);
		}

		// Destroy Surface
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	}

	void VulkanRenderer::Render()
	{
		// Frame Time Calculation Start
		{
			//auto start = std::chrono::high_resolution_clock::now();
		}

		VulkanFrameData& frameData = m_Frames[m_CurrentFrameIndex];
		VkCommandBufferBeginInfo beginInfo{};
		
		// Frame Preparation
		{
			vkWaitForFences(m_Device->handle, 1, &frameData.computeFence, VK_TRUE, UINT64_MAX);
			vkResetFences(m_Device->handle, 1, &frameData.computeFence);

			vkWaitForFences(m_Device->handle, 1, &frameData.graphicsFence, VK_TRUE, UINT64_MAX);
			vkResetFences(m_Device->handle, 1, &frameData.graphicsFence);

			frameData.computeCmdPool.Reset();
			frameData.graphicsCmdPool.Reset();

			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		}

		// Retrieve Next Swapchain Image
		vkAcquireNextImageKHR(m_Device->handle, m_Swapchain.handle, UINT64_MAX, frameData.swapchainSemaphore, VK_NULL_HANDLE, &m_Swapchain.imageIndex);
		VkImage& swapImage = m_Swapchain.images[m_Swapchain.imageIndex];
		VkImageView& swapImageView = m_Swapchain.imageViews[m_Swapchain.imageIndex];
		const VkExtent2D& swapExtent = m_Swapchain.extent;

		// Compute Command Recording
		{
			vkBeginCommandBuffer(frameData.computeCmdBuffer, &beginInfo);

			m_ImageProcessor.AddLayoutTransition(
				m_RenderImage.image,
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL,
				VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
				0, VK_ACCESS_2_MEMORY_WRITE_BIT
			);

			m_ImageProcessor.TransitionLayouts(frameData.computeCmdBuffer);
			m_ImageProcessor.ClearTransitions();

			// Compute Commands
			{
				/*vkCmdBindPipeline(frameData.computeCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_Pipelines.compute->Get());

				vkCmdBindDescriptorSets(frameData.computeCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_Pipelines.compute->GetLayout(), 0, 1, &m_Pipelines.computeDescriptor->GetSet(), 0, nullptr);

				vkCmdDispatch(
					frameData.computeCmdBuffer,
					static_cast<uint32_t>(std::ceil(m_Swapchain.extent.width) / 16.f),
					static_cast<uint32_t>(std::ceil(m_Swapchain.extent.height) / 16.f),
					1
				);*/
			}

			vkEndCommandBuffer(frameData.computeCmdBuffer);
		}

		// Graphics Command Recording
		{
			vkBeginCommandBuffer(frameData.graphicsCmdBuffer, &beginInfo);

			// General Graphics Commands
			{

			}

			// Prepare Render Image for UI rendering
			{
				if (m_RenderAsUITexture)
				{
					// Transfer render image to shader read only (To use as a texture)
					m_ImageProcessor.AddLayoutTransition(
						m_RenderImage.image,
						VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
						VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
						VK_ACCESS_2_MEMORY_WRITE_BIT, VK_ACCESS_2_MEMORY_READ_BIT
					);

					m_ImageProcessor.AddLayoutTransition(
						swapImage,
						VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
						VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
						0, VK_ACCESS_2_MEMORY_WRITE_BIT
					);

					// Finalize image layout transitions
					m_ImageProcessor.TransitionLayouts(frameData.graphicsCmdBuffer);
					m_ImageProcessor.ClearTransitions();
				}
				else
				{
					// Copy Image to swapchain
					{
						m_ImageProcessor.AddLayoutTransition(
							m_RenderImage.image,
							VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
							VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
							VK_ACCESS_2_MEMORY_WRITE_BIT, VK_ACCESS_2_MEMORY_READ_BIT
						);

						m_ImageProcessor.AddLayoutTransition(
							swapImage,
							VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
							VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
							0, VK_ACCESS_2_MEMORY_WRITE_BIT
						);

						// Finalize image layout transitions
						m_ImageProcessor.TransitionLayouts(frameData.graphicsCmdBuffer);
						m_ImageProcessor.ClearTransitions();

						m_ImageProcessor.CopyImage(frameData.graphicsCmdBuffer, m_RenderImage.image, swapImage, { m_RenderImage.extent.width,m_RenderImage.extent.height }, swapExtent);
					}

					// Transition Swapchain image into writable format
					{
						// Transfer swapchain image to color attachment for ImGui
						m_ImageProcessor.AddLayoutTransition(
							swapImage,
							VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
							VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
							VK_ACCESS_2_MEMORY_WRITE_BIT, VK_ACCESS_2_MEMORY_WRITE_BIT
						);

						// Finalize image layout transitions
						m_ImageProcessor.TransitionLayouts(frameData.graphicsCmdBuffer);
						m_ImageProcessor.ClearTransitions();
					}
				}
			}

			// Submit ImGui Render Commands
			RenderImGui(frameData.graphicsCmdBuffer, swapImageView);

			// Post UI Image Transitions
			{
				// Transfer swapchain image to presentation source
				m_ImageProcessor.AddLayoutTransition(
					swapImage,
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
					VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
					VK_ACCESS_2_MEMORY_WRITE_BIT, VK_ACCESS_2_MEMORY_READ_BIT
				);

				// Finalize image layout transitions
				m_ImageProcessor.TransitionLayouts(frameData.graphicsCmdBuffer);
				m_ImageProcessor.ClearTransitions();
			}

			vkEndCommandBuffer(frameData.graphicsCmdBuffer);
		}

		// Submit Render Data
		{
			VkCommandBufferSubmitInfo computeBufferInfo{};
			computeBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
			computeBufferInfo.commandBuffer = frameData.computeCmdBuffer;

			VkSemaphoreSubmitInfo computeSemaphoreInfo{};
			computeSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
			computeSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
			computeSemaphoreInfo.semaphore = frameData.computeSemaphore;

			VkSubmitInfo2 computeSubmitInfo{};
			computeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
			computeSubmitInfo.commandBufferInfoCount = 1;
			computeSubmitInfo.pCommandBufferInfos = &computeBufferInfo;
			computeSubmitInfo.signalSemaphoreInfoCount = 1;
			computeSubmitInfo.pSignalSemaphoreInfos = &computeSemaphoreInfo;

			VkCommandBufferSubmitInfo graphicsBufferInfo{};
			graphicsBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
			graphicsBufferInfo.commandBuffer = frameData.graphicsCmdBuffer;

			VkSemaphoreSubmitInfo signalSemaphoreInfo{};
			signalSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
			signalSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
			signalSemaphoreInfo.semaphore = frameData.graphicsSemaphore;

			VkSemaphoreSubmitInfo waitSemaphoreInfo{};
			waitSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
			waitSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
			waitSemaphoreInfo.semaphore = frameData.swapchainSemaphore;

			VkSubmitInfo2 graphicsSubmitInfo{};
			graphicsSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;

			graphicsSubmitInfo.commandBufferInfoCount = 1;
			graphicsSubmitInfo.pCommandBufferInfos = &graphicsBufferInfo;

			graphicsSubmitInfo.signalSemaphoreInfoCount = 1;
			graphicsSubmitInfo.pSignalSemaphoreInfos = &signalSemaphoreInfo;

			VkSemaphoreSubmitInfo waitSemaphores[] = { computeSemaphoreInfo, waitSemaphoreInfo };
			graphicsSubmitInfo.waitSemaphoreInfoCount = 2;
			graphicsSubmitInfo.pWaitSemaphoreInfos = waitSemaphores;

			VkPresentInfoKHR presentInfo{};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.pSwapchains = &m_Swapchain.handle;
			presentInfo.swapchainCount = 1;
			presentInfo.pImageIndices = &m_Swapchain.imageIndex;
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = &frameData.graphicsSemaphore;

			// Submit Compute Queue
			vkQueueSubmit2(m_ComputeQueue, 1, &computeSubmitInfo, frameData.computeFence);

			// Submit Graphics Queue
			vkQueueSubmit2(m_GraphicsQueue, 1, &graphicsSubmitInfo, frameData.graphicsFence);

			// Present Frame
			vkQueuePresentKHR(m_PresentQueue, &presentInfo);
		}

		// Step into next frame
		m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % g_Max_Frames_In_Flight;

		// Frame Time Calculation End
		{
			/*auto end = std::chrono::high_resolution_clock::now();
			auto frameTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.f;
			INDY_CORE_INFO("Frame Time: {0}ms", frameTime);*/
		}
	}

	void VulkanRenderer::Enable()
	{
		//m_Enabled = true;
	}

	void VulkanRenderer::Disable()
	{
		//m_Enabled = false;
	}

	void VulkanRenderer::BuildPipelines()
	{
		INDY_CORE_WARN("Building Vulkan Pipelines...");
		VulkanPipelineBuilder builder(m_Device->handle);
		
		INDY_CORE_WARN("Building Compute Pipeline...");
		// Build Compute Pipeline
		{
			//Temp
			PipelineBuildOptions options{};
			options.type = INDY_PIPELINE_TYPE_COMPUTE;

			builder.BindShader("shaders/gradient.glsl.comp");
			builder.Build(&options);
			m_ComputePipeline = builder.GetPipeline();

			builder.Clear();
		}

		INDY_CORE_WARN("Done!");
		// Build Graphics Pipeline
		{
			/*buildOptions.type = INDY_PIPELINE_TYPE_GRAPHICS;

			VulkanShader vertexShader("shaders/vertex.glsl.vert");
			builder.BindShader(vertexShader, INDY_PIPELINE_SHADER_STAGE_VERTEX);

			VulkanShader  fragmentShader("shaders/fragmet.glsl.frag");
			builder.BindShader(fragmentShader, INDY_PIPELINE_SHADER_STAGE_FRAGMENT);

			m_GraphicsPipeline = *static_cast<VulkanPipeline*>(builder.Build(buildOptions));
			builder.Clear();*/
		}

		// Old Impl -----------------------
		// --------------------------------

		//// Descriptor Pool Initialization
		//std::vector<VulkanDescriptorPool::Ratio> sizes = {
		//	{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 }  // 1 descriptor for each storage image (for compute)
		//};
		//m_Pipelines.descriptorPool = std::make_unique<VulkanDescriptorPool>(m_Device->handle, 10, sizes);

		//// Pipeline Layout Builder
		//VulkanDescriptorLayoutBuilder layoutBuilder;

		//{ // Compute Pipeline

		//	// Descriptor Set Layout for compute shader
		//	layoutBuilder.AddBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0, 1); // Binding 0 is the image the compute shader uses

		//	// Build descriptor set layout
		//	VkDescriptorSetLayout layout = layoutBuilder.Build(m_Device->handle, { VK_SHADER_STAGE_COMPUTE_BIT });
		//	layoutBuilder.Clear();

		//	// Get Compute Shader
		//	Shader computeShader("shaders/gradient.glsl.comp");
		//	
		//	PipelineBuildOptions options{};

		//	VulkanPipelineBuilder builder;
		//	builder.BindShader(computeShader);
		//	builder.BindDescriptorSetLayout(INDY_SHADERINDY_SHADER_TYPE_COMPUTE, /* Pipeline Descriptor Pool */, );
		//	m_ComputePipeline = builder.Build<VulkanPipeline>();

		//	// Pipeline
		//	m_Pipelines.compute = std::make_unique<VulkanPipeline>(m_Device->handle, VulkanPipelineInfo(INDY_PIPELINE_TYPE_COMPUTE));
		//	m_Pipelines.compute->BindShader(computeShader);
		//	m_Pipelines.compute->BindDescriptorSetLayout(INDY_SHADER_TYPE_COMPUTE, *m_Pipelines.descriptorPool, layout);
		//	m_Pipelines.compute->Build();

		//	// Get pipeline descriptor
		//	m_Pipelines.computeDescriptor = m_Pipelines.compute->GetDescriptor(INDY_SHADER_TYPE_COMPUTE);

		//	// Render Image Info
		//	VkDescriptorImageInfo imageInfo{};
		//	imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		//	imageInfo.imageView = m_RenderImage.view;

		//	// Attach render image to compute pipeline descriptor set
		//	m_Pipelines.computeDescriptor->UpdateImageBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0, &imageInfo);
		//	m_Pipelines.computeDescriptor->UpdateDescriptorSets(m_Device->handle);
		//}
	}

	void VulkanRenderer::InitImGui()
	{
		ImGui::CreateContext();

		std::vector<VkDescriptorPoolSize> poolSizes =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER,					1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,				1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,				1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,		1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,		1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,			1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,			1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,	1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,	1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,			1000 }
		};

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.maxSets = 1000;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();

		vkCreateDescriptorPool(m_Device->handle, &poolInfo, nullptr, &m_ImGuiDescriptorPool);

		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontDefault();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		ImGui::StyleColorsDark();

		// Initialize ImGui for GLFW
		if (!ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow*>(m_Window->NativeWindow()), true))
		{
			INDY_CORE_ERROR("ImGui Initialization Error: Failed to initialize ImGui for GLFW");
			return;
		}

		// this initializes imgui for Vulkan
		ImGui_ImplVulkan_InitInfo initInfo = {};
		initInfo.Instance = m_Instance;
		initInfo.PhysicalDevice = m_Device->physicalDevice;
		initInfo.Device = m_Device->handle;
		initInfo.QueueFamily = m_Device->queueFamilies.graphics.value();
		initInfo.Queue = m_GraphicsQueue;
		initInfo.DescriptorPool = m_ImGuiDescriptorPool;
		initInfo.MinImageCount = 3;
		initInfo.ImageCount = 3;
		initInfo.UseDynamicRendering = true;

		// Rendering Pipeline info
		VkPipelineRenderingCreateInfoKHR pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		pipelineInfo.colorAttachmentCount = 1;
		pipelineInfo.pColorAttachmentFormats = &m_Swapchain.format.format;
		pipelineInfo.pNext = nullptr;

		// Dynamic rendering parameters
		initInfo.PipelineRenderingCreateInfo = pipelineInfo;

		initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		
		if (!ImGui_ImplVulkan_Init(&initInfo))
		{
			INDY_CORE_ERROR("Error Initializing Vulkan ImGui Impl");
			return;
		}

		if (!ImGui_ImplVulkan_CreateFontsTexture())
		{
			INDY_CORE_ERROR("Error Initializing Vulkan ImGui font texture");
			return;
		}
	}

	void VulkanRenderer::RenderImGui(const VkCommandBuffer& cmdBuffer, const VkImageView& imageView)
	{
		// Start the Dear ImGui frame
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// ImGui User Draw Data
		{
			if (m_RenderAsUITexture)
			{
				ImGuiViewport* viewport = ImGui::GetMainViewport();

				ImGui::SetNextWindowPos(viewport->Pos);
				ImGui::SetNextWindowSize(viewport->Size);

				ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration;

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
				ImGui::Begin("Main Viewport", nullptr, windowFlags);
				ImGui::Image((ImTextureID)m_ImGuiRenderImageDescriptor, viewport->Size);
				ImGui::End();
				ImGui::PopStyleVar(2);
			}

			ImGui::ShowDemoWindow();
		}

		ImGui::Render();

		VkRenderingAttachmentInfo colorAttachment{};
		colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		colorAttachment.pNext = nullptr;

		colorAttachment.imageView = imageView;
		colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachment.loadOp = m_RenderAsUITexture ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		// Set render area to the extent of the rendered image (window)
		VkRect2D renderArea{};
		renderArea.extent = m_Swapchain.extent;

		VkRenderingInfo renderInfo{};
		renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderInfo.colorAttachmentCount = 1;
		renderInfo.pColorAttachments = &colorAttachment;
		renderInfo.renderArea = renderArea;
		renderInfo.flags = 0;
		renderInfo.layerCount = 1;

		vkCmdBeginRendering(cmdBuffer, &renderInfo);

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer);

		vkCmdEndRendering(cmdBuffer);

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}
}