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

import Indy.Graphics;
import Indy.VulkanGraphics;
import Indy.Window;

namespace Indy
{
	VulkanRenderer::VulkanRenderer(Window* window, const VkInstance& instance)
		: m_ID(window->Properties().id), m_CurrentFrameIndex(0), m_Instance(instance), m_Enabled(true)
	{
		// Generate the surface to present to
		if (glfwCreateWindowSurface(instance, static_cast<GLFWwindow*>(window->NativeWindow()), nullptr, &m_Surface) != VK_SUCCESS)
		{
			INDY_CORE_CRITICAL("Failed to create window surface!");
			return;
		}

		// Choose a physical device based on the following compatibility
		VulkanDeviceCompatibility compat;
		compat.geometryShader = COMPAT_REQUIRED;				// always require geometry shaders
		compat.graphics = COMPAT_REQUIRED;						// always require graphics operations
		compat.compute = COMPAT_PREFER;							// always prefer compute shaders, but don't require it.
		compat.type = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;		// always assume dedicated GPU
		compat.typePreference = COMPAT_PREFER;					// always prefer a dedicated GPU, but it's not required.

		m_PhysicalDevice = VulkanDevice::ChoosePhysicalDevice(compat);

		// Ensure the physical device is capable of presenting
		if (!VulkanDevice::GetGPUSurfaceSupport(m_PhysicalDevice, m_Surface))
		{
			INDY_CORE_ERROR("Cannot Create Renderer! Selected GPU does not support presenting to a Vulkan Surface!");
			return;
		}

		// Create the logical device for this renderer
		m_Device = std::make_unique<VulkanDevice>(instance, m_PhysicalDevice);

		// Retrieve the new device references
		m_LogicalDevice = m_Device->Get();
		m_PhysicalDevice = m_Device->GetPhysicalDevice();
		m_ImageAllocator = m_Device->GetVmaAllocator();

		// Setup a swapchain for this window
		m_Swapchain = std::make_unique<VulkanSwapchain>(m_PhysicalDevice, m_LogicalDevice, m_Surface, window);

		// Retrieve Vulkan Queue Handles
		{
			VkDeviceQueueInfo2 queueGetInfo{};
			queueGetInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
			queueGetInfo.pNext = nullptr;
			queueGetInfo.flags = 0;
			queueGetInfo.queueIndex = 0;

			// Retrieve Compute Queue
			queueGetInfo.queueFamilyIndex = m_PhysicalDevice.queueFamilies.compute.value();
			vkGetDeviceQueue2(m_LogicalDevice, &queueGetInfo, &m_ComputeQueue);

			// Retrieve Graphics Queue
			queueGetInfo.queueFamilyIndex = m_PhysicalDevice.queueFamilies.graphics.value();
			vkGetDeviceQueue2(m_LogicalDevice, &queueGetInfo, &m_GraphicsQueue);

			// Retrieve Present Queue (NOTE: This can be the same as the graphics queue)
			queueGetInfo.queueFamilyIndex = m_PhysicalDevice.queueFamilies.present.value();
			vkGetDeviceQueue2(m_LogicalDevice, &queueGetInfo, &m_PresentQueue);
		}

		// Allocate resources for the maximum number of 'in-flight' frames
		m_Frames.resize(g_Max_Frames_In_Flight);
		for (auto& frame : m_Frames)
			AllocateVulkanFrameData(m_LogicalDevice, m_PhysicalDevice.queueFamilies, frame);

		// Immediate Command Setup
		{
			// Initialize immediate command pool
			m_ImmediateCmdPool.Allocate(m_LogicalDevice, m_PhysicalDevice.queueFamilies.graphics.value(), 1);
			m_ImmediateCmdBuffer = m_ImmediateCmdPool.GetCommandBuffer(0);

			// Create the fence for the immediate command pool
			VkFenceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			// Immediate Fence
			if (vkCreateFence(m_LogicalDevice, &createInfo, nullptr, &m_ImmediateFence) != VK_SUCCESS)
			{
				INDY_CORE_ERROR("Failed to create sync object [VkFence]!");
				return;
			}
		}

		

		// Create Render Image (NOTE: Likely needs to happen again if the window resizes)
		{
			// Create the image to render to
			VulkanImageSpec imageSpec{};
			imageSpec.extent.width = window->Properties().width;
			imageSpec.extent.height = window->Properties().height;
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
			m_RenderImage = VulkanImage::Create(m_LogicalDevice, m_ImageAllocator, imageSpec);
		}

		// Build Render Pipelines
		BuildPipelines();

		// Initialize ImGUI for this renderer
		InitImGui(window, instance);

		// Create the image descriptor for ImGui's backend
		m_RenderImage.imguiDescriptor = ImGui_ImplVulkan_AddTexture(m_RenderImage.sampler, m_RenderImage.view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	VulkanRenderer::~VulkanRenderer()
	{
		vkDeviceWaitIdle(m_LogicalDevice);

		// Shutdown ImGui
		ImGui_ImplVulkan_RemoveTexture(m_RenderImage.imguiDescriptor);
		ImGui_ImplVulkan_Shutdown();
		vkDestroyDescriptorPool(m_LogicalDevice, m_ImGuiDescriptorPool, nullptr);

		// Explicitly delete all frames
		m_Frames.clear();

		// Explicitly destroy offscreen image
		vkDestroySampler(m_LogicalDevice, m_RenderImage.sampler, nullptr);
		vkDestroyImageView(m_LogicalDevice, m_RenderImage.view, nullptr);
		vmaDestroyImage(m_ImageAllocator, m_RenderImage.image, m_RenderImage.allocation);

		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	}

	void VulkanRenderer::Render()
	{
		auto start = std::chrono::high_resolution_clock::now();

		VulkanFrameData& frameData = m_Frames[m_CurrentFrameIndex];
		VkCommandBufferBeginInfo beginInfo{};
		
		// Frame Preparation
		{
			vkWaitForFences(m_LogicalDevice, 1, &frameData.computeFence, VK_TRUE, UINT64_MAX);
			vkResetFences(m_LogicalDevice, 1, &frameData.computeFence);

			vkWaitForFences(m_LogicalDevice, 1, &frameData.graphicsFence, VK_TRUE, UINT64_MAX);
			vkResetFences(m_LogicalDevice, 1, &frameData.graphicsFence);

			frameData.computeCmdPool.Reset();
			frameData.graphicsCmdPool.Reset();

			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		}

		// Retrieve Next Swapchain Image
		vkAcquireNextImageKHR(m_LogicalDevice, m_Swapchain->Get(), UINT64_MAX, frameData.swapchainSemaphore, VK_NULL_HANDLE, &m_SwapchainImageIndex);
		VkImage& swapImage = m_Swapchain->GetImage(m_SwapchainImageIndex).image;
		VkImageView& swapImageView = m_Swapchain->GetImage(m_SwapchainImageIndex).imageView;
		const VkExtent2D& swapExtent = m_Swapchain->GetExtent();

		vkBeginCommandBuffer(frameData.computeCmdBuffer, &beginInfo);

		// Initial Image Transition (Must be in writable format)
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
			vkCmdBindPipeline(frameData.computeCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_Pipelines.compute->Get());

			vkCmdBindDescriptorSets(frameData.computeCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_Pipelines.compute->GetLayout(), 0, 1, &m_Pipelines.computeDescriptor->GetSet(), 0, nullptr);

			vkCmdDispatch(
				frameData.computeCmdBuffer,
				static_cast<uint32_t>(std::ceil(m_Swapchain->GetExtent().width) / 16.f),
				static_cast<uint32_t>(std::ceil(m_Swapchain->GetExtent().height) / 16.f),
				1
			);
		}

		vkEndCommandBuffer(frameData.computeCmdBuffer);

		vkBeginCommandBuffer(frameData.graphicsCmdBuffer, &beginInfo);

		// General Graphics Commands
		{

		}

		// Transfer render image to shader read only (For rendering as ImGui texture)
		m_ImageProcessor.AddLayoutTransition(
			m_RenderImage.image,
			VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
			VK_ACCESS_2_MEMORY_WRITE_BIT, VK_ACCESS_2_MEMORY_READ_BIT
		);

		// Transfer swapchain image to color attachment for ImGui
		m_ImageProcessor.AddLayoutTransition(
			swapImage,
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			0, VK_ACCESS_2_MEMORY_WRITE_BIT
		);

		m_ImageProcessor.TransitionLayouts(frameData.graphicsCmdBuffer);
		m_ImageProcessor.ClearTransitions();

		// UI Commands
		{
			// Record ImGui Stuff
			RenderImGui(frameData.graphicsCmdBuffer, swapImageView);
		}

		// Transfer swapchain image to presentation
		m_ImageProcessor.AddLayoutTransition(
			swapImage,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
			VK_ACCESS_2_MEMORY_WRITE_BIT, VK_ACCESS_2_MEMORY_READ_BIT
		);

		// Finalize image layout transitions
		m_ImageProcessor.TransitionLayouts(frameData.graphicsCmdBuffer);
		m_ImageProcessor.ClearTransitions();

		vkEndCommandBuffer(frameData.graphicsCmdBuffer);

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
		presentInfo.pSwapchains = &m_Swapchain->Get();
		presentInfo.swapchainCount = 1;
		presentInfo.pImageIndices = &m_SwapchainImageIndex;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &frameData.graphicsSemaphore;

		// Submit Compute Queue
		vkQueueSubmit2(m_ComputeQueue, 1, &computeSubmitInfo, frameData.computeFence);

		// Submit Graphics Queue
		vkQueueSubmit2(m_GraphicsQueue, 1, &graphicsSubmitInfo, frameData.graphicsFence);

		// Present Frame
		vkQueuePresentKHR(m_PresentQueue, &presentInfo);

		// Step into next frame
		m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % g_Max_Frames_In_Flight;

		auto end = std::chrono::high_resolution_clock::now();
		auto frameTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.f;

		// INDY_CORE_INFO("Frame Time: {0}ms", frameTime);
	}

	void VulkanRenderer::Enable()
	{
		m_Enabled = true;
	}

	void VulkanRenderer::Disable()
	{
		m_Enabled = false;
	}

	void VulkanRenderer::BuildPipelines()
	{
		// TODO: Eventually, I'd like to make it such that users can build their own pipelines for a renderer, rather than use default stuff

		// Render Image Info (for binding to shaders)
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imageInfo.imageView = m_RenderImage.view;

		// Descriptor Pool Initialization
		std::vector<VulkanDescriptorPool::Ratio> sizes = {
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 }  // 1 descriptor for each storage image (for compute)
		};

		m_Pipelines.descriptorPool = std::make_unique<VulkanDescriptorPool>(m_LogicalDevice, 10, sizes);

		// Pipeline Layout Builder
		VulkanDescriptorLayoutBuilder layoutBuilder;

		{ // Compute Pipeline

			// Descriptor Set Layout for compute shader
			layoutBuilder.AddBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0, 1); // Binding 0 is the image the compute shader uses

			// Build descriptor set layout
			VkDescriptorSetLayout layout = layoutBuilder.Build(m_LogicalDevice, { VK_SHADER_STAGE_COMPUTE_BIT });
			layoutBuilder.Clear();

			// Get Compute Shader
			Shader computeShader(INDY_SHADER_TYPE_COMPUTE, INDY_SHADER_FORMAT_GLSL, "shaders/gradient.glsl.comp");

			// Pipeline
			m_Pipelines.compute = std::make_unique<VulkanPipeline>(m_LogicalDevice, VulkanPipelineInfo(INDY_PIPELINE_TYPE_COMPUTE));
			m_Pipelines.compute->BindShader(computeShader);
			m_Pipelines.compute->BindDescriptorSetLayout(INDY_SHADER_TYPE_COMPUTE, *m_Pipelines.descriptorPool, layout);
			m_Pipelines.compute->Build();

			// Get pipeline descriptor
			m_Pipelines.computeDescriptor = m_Pipelines.compute->GetDescriptor(INDY_SHADER_TYPE_COMPUTE);

			// Attach render image to compute pipeline descriptor set
			m_Pipelines.computeDescriptor->UpdateImageBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0, &imageInfo);
			m_Pipelines.computeDescriptor->UpdateDescriptorSets(m_LogicalDevice);
		}
	}

	void VulkanRenderer::InitImGui(Window* window, const VkInstance& instance)
	{
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

		vkCreateDescriptorPool(m_LogicalDevice, &poolInfo, nullptr, &m_ImGuiDescriptorPool);

		ImGui::CreateContext();

		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontDefault();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		// Initialize ImGui for GLFW
		if (!ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow*>(window->NativeWindow()), true))
		{
			INDY_CORE_ERROR("ImGui Initialization Error: Failed to initialize ImGui for GLFW");
			return;
		}

		// this initializes imgui for Vulkan
		ImGui_ImplVulkan_InitInfo initInfo = {};
		initInfo.Instance = instance;
		initInfo.PhysicalDevice = m_PhysicalDevice.handle;
		initInfo.Device = m_LogicalDevice;
		initInfo.QueueFamily = m_PhysicalDevice.queueFamilies.graphics.value();
		initInfo.Queue = m_GraphicsQueue;
		initInfo.DescriptorPool = m_ImGuiDescriptorPool;
		initInfo.MinImageCount = 3;
		initInfo.ImageCount = 3;
		initInfo.UseDynamicRendering = true;

		// Rendering Pipeline info
		VkPipelineRenderingCreateInfoKHR pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		pipelineInfo.colorAttachmentCount = 1;
		pipelineInfo.pColorAttachmentFormats = &m_Swapchain->GetFormat().format;
		pipelineInfo.pNext = nullptr;

		// Dynamic rendering parameters
		initInfo.PipelineRenderingCreateInfo = pipelineInfo;

		initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		if (!ImGui_ImplVulkan_Init(&initInfo))
		{
			INDY_CORE_ERROR("Error Initializing Vulkan ImGui Impl");
		}

		if (!ImGui_ImplVulkan_CreateFontsTexture())
		{
			INDY_CORE_ERROR("Error Initializing Vulkan ImGui font texture");
		}
	}

	void VulkanRenderer::RenderImGui(const VkCommandBuffer& cmdBuffer, const VkImageView& imageView)
	{
		// Start the Dear ImGui frame
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGuiViewport* viewport = ImGui::GetMainViewport();

		// Demo Window
		ImGui::ShowDemoWindow();

		// User-Created ImGui Windows
		{
			ImGui::Begin("Hello World", nullptr);
			ImGui::Text("pointer = %p", m_RenderImage.imguiDescriptor);
			ImGui::Text("size = %d x %d", m_RenderImage.extent.width, m_RenderImage.extent.height);
			ImGui::Image((ImTextureID)m_RenderImage.imguiDescriptor, ImVec2(640, 380));
			ImGui::End();
		}

		ImGui::Render();

		VkRenderingAttachmentInfo colorAttachment{};
		colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		colorAttachment.pNext = nullptr;

		colorAttachment.imageView = imageView;
		colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		// Set render area to the extent of the rendered image (window)
		VkRect2D renderArea{};
		renderArea.extent = m_Swapchain->GetExtent();

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