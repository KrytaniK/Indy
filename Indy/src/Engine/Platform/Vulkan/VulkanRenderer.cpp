#include <Engine/Core/LogMacros.h>

#include <memory>
#include <vector>

#include <chrono>
#include <iostream>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#define GLM_FORCE_RADIANS
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

import Indy.Graphics;
import Indy.VulkanGraphics;
import Indy.Window;

import Indy.Profiler;

namespace Indy
{
	void VulkanRenderer::SubmitImmediate(const std::function<void(const VkCommandBuffer&)>& command)
	{
		if (VulkanRenderer::s_Renderer)
			s_Renderer->SubmitImmediateCommand(command);
	}

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

		// Create the resource allocator
		{
			VmaAllocatorCreateInfo allocatorInfo{};
			allocatorInfo.physicalDevice = m_Device->physicalDevice;
			allocatorInfo.device = m_Device->handle;
			allocatorInfo.instance = m_Instance;
			allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT; // or other flags

			if (vmaCreateAllocator(&allocatorInfo, &m_ResourceAllocator) != VK_SUCCESS)
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
			m_RenderImage = VulkanImage::Create(m_Device->handle, m_ResourceAllocator, imageSpec);
		}

		// Build Render Pipelines
		BuildPipelines();

		// Initialize ImGUI for this renderer
		InitImGui();

		// Create the image descriptor for ImGui's backend
		m_ImGuiRenderImageDescriptor = ImGui_ImplVulkan_AddTexture(m_RenderImage.sampler, m_RenderImage.view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		
		// Set static renderer
		VulkanRenderer::s_Renderer = this;

		// -------------------------------
		// -------------------------------
		// Temp Rectangle Data -----------
		// -------------------------------
		// -------------------------------

		m_Rectangle.vertices.resize(4);
		m_Rectangle.indices.resize(6);

		// Note: This is in SCREEN SPACE
		m_Rectangle.vertices[0].position = { 0.75,-0.5, 0 };
		m_Rectangle.vertices[1].position = { 0.75,0.5, 0 };
		m_Rectangle.vertices[2].position = { -0.75,-0.5, 0 };
		m_Rectangle.vertices[3].position = { -0.75,0.5, 0 };

		m_Rectangle.vertices[0].color = { 1, 0, 0, 1 };
		m_Rectangle.vertices[1].color = { 0, 1, 0 ,1 };
		m_Rectangle.vertices[2].color = { 0, 0, 1 ,1 };
		m_Rectangle.vertices[3].color = { 0, 1, 1 ,1 };

		m_Rectangle.indices[0] = 0;
		m_Rectangle.indices[1] = 1;
		m_Rectangle.indices[2] = 2;

		m_Rectangle.indices[3] = 2;
		m_Rectangle.indices[4] = 1;
		m_Rectangle.indices[5] = 3;

		m_RectMeshData = VulkanMeshData::Create(m_Rectangle, m_Device->handle, m_ResourceAllocator);
	}

	VulkanRenderer::~VulkanRenderer()
	{
		// Wait for graphics operations to finish
		vkDeviceWaitIdle(m_Device->handle);

		// Temp destroy mesh buffers
		VulkanBuffer::Destroy(m_ResourceAllocator, m_RectMeshData.vertexBuffer);
		VulkanBuffer::Destroy(m_ResourceAllocator, m_RectMeshData.indexBuffer);

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
			vmaDestroyImage(m_ResourceAllocator, m_RenderImage.image, m_RenderImage.allocation);
			vmaDestroyAllocator(m_ResourceAllocator);
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
		//auto start = std::chrono::high_resolution_clock::now();

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
				vkCmdBindPipeline(frameData.computeCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_ComputePipeline.pipeline);

				vkCmdBindDescriptorSets(frameData.computeCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_ComputePipeline.layout, 0, 1, &m_ComputeDescriptorSet, 0, nullptr);

				vkCmdDispatch(
					frameData.computeCmdBuffer,
					static_cast<uint32_t>(std::ceil(m_Swapchain.extent.width) / 16.f),
					static_cast<uint32_t>(std::ceil(m_Swapchain.extent.height) / 16.f),
					1
				);
			}

			vkEndCommandBuffer(frameData.computeCmdBuffer);
		}

		// Graphics Command Recording
		{
			vkBeginCommandBuffer(frameData.graphicsCmdBuffer, &beginInfo);

			// Transition image to correct layout for writing
			m_ImageProcessor.AddLayoutTransition(
				m_RenderImage.image,
				VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
				VK_ACCESS_2_MEMORY_WRITE_BIT, VK_ACCESS_2_MEMORY_WRITE_BIT
			);

			m_ImageProcessor.TransitionLayouts(frameData.graphicsCmdBuffer);
			m_ImageProcessor.ClearTransitions();

			// Begin a render pass
			VkRenderingAttachmentInfo colorAttachment{};
			colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			colorAttachment.imageView = m_RenderImage.view;
			colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

			VkExtent2D drawExtent = { m_RenderImage.extent.width, m_RenderImage.extent.height };

			VkRenderingInfo renderInfo{};
			renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
			renderInfo.renderArea.extent = drawExtent;
			renderInfo.colorAttachmentCount = 1;
			renderInfo.pColorAttachments = &colorAttachment;
			renderInfo.flags = 0;
			renderInfo.layerCount = 2;

			// General Graphics Commands
			{
				vkCmdBeginRendering(frameData.graphicsCmdBuffer, &renderInfo);

				vkCmdBindPipeline(frameData.graphicsCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline.pipeline);

				//set dynamic viewport and scissor
				VkViewport viewport = {};
				viewport.x = 0;
				viewport.y = 0;
				viewport.width = drawExtent.width;
				viewport.height = drawExtent.height;
				viewport.minDepth = 0.f;
				viewport.maxDepth = 1.f;

				vkCmdSetViewport(frameData.graphicsCmdBuffer, 0, 1, &viewport);

				VkRect2D scissor = {};
				scissor.offset.x = 0;
				scissor.offset.y = 0;
				scissor.extent.width = drawExtent.width;
				scissor.extent.height = drawExtent.height;

				vkCmdSetScissor(frameData.graphicsCmdBuffer, 0, 1, &scissor);

				VulkanMeshPushConstants pushConstants{};
				pushConstants.worldMatrix = glm::mat4(1.f); // Note: This is in SCREEN SPACE
				pushConstants.vertexBufferAddress = m_RectMeshData.vertexBufferAddress;

				vkCmdPushConstants(frameData.graphicsCmdBuffer, m_GraphicsPipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VulkanMeshPushConstants), &pushConstants);
				vkCmdBindIndexBuffer(frameData.graphicsCmdBuffer, m_RectMeshData.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

				vkCmdDrawIndexed(frameData.graphicsCmdBuffer, 6, 1, 0, 0, 0);

				vkCmdEndRendering(frameData.graphicsCmdBuffer);
			
			}

			// Prepare Render Image for UI rendering
			{
				if (m_RenderAsUITexture)
				{
					// Transfer render image to shader read only (To use as a texture)
					m_ImageProcessor.AddLayoutTransition(
						m_RenderImage.image,
						VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
						VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
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

	void VulkanRenderer::SubmitImmediateCommand(const std::function<void(const VkCommandBuffer&)>& command)
	{
		vkResetFences(m_Device->handle, 1, &m_ImmediateFence);

		m_ImmediateCmdPool.Reset();
		m_ImmediateCmdPool.BeginCommandBuffer(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		VkCommandBuffer cmdBuffer = m_ImmediateCmdPool.GetCommandBuffer(0);

		command(cmdBuffer);

		m_ImmediateCmdPool.EndCommandBuffer(0);

		VkCommandBufferSubmitInfo cmdSubmitInfo{};
		cmdSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
		cmdSubmitInfo.commandBuffer = cmdBuffer;

		VkSubmitInfo2 submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &cmdSubmitInfo;

		vkQueueSubmit2(m_GraphicsQueue, 1, &submitInfo, m_ImmediateFence);
		vkWaitForFences(m_Device->handle, 1, &m_ImmediateFence, VK_TRUE, UINT64_MAX);
	}

	void VulkanRenderer::BuildPipelines()
	{
		INDY_CORE_WARN("Building Vulkan Pipelines...");
		VulkanPipelineBuilder builder(m_Device->handle);

		// Temp Build Options
		VulkanPipelineBuildOptions options{};
		options.type = INDY_PIPELINE_TYPE_COMPUTE;
		
		INDY_CORE_WARN("Building Compute Pipeline...");
		// Build Compute Pipeline
		{
			// Bind Shaders
			builder.BindShader("shaders/gradient.glsl.comp");

			// Build and retrieve pipeline
			builder.Build(&options);
			m_ComputePipeline = builder.GetPipeline();

			// Allocate any descriptor sets
			VulkanDescriptorSetAllocator dsAllocator(m_ComputePipeline.descriptorPool);
			m_ComputeDescriptorSet = dsAllocator.Allocate(m_Device->handle, m_ComputePipeline.descriptorSetLayout);

			// Render Image Info
			VkDescriptorImageInfo imageInfos{};
			imageInfos.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			imageInfos.imageView = m_RenderImage.view;

			// Update Descriptor Set Write Information
			VulkanDescriptorUpdateInfo updateInfo;
			updateInfo.set = m_ComputeDescriptorSet;
			updateInfo.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			updateInfo.binding = 0;
			updateInfo.count = 1;
			updateInfo.elementOffset = 0;

			// Update Descriptor Set
			VulkanDescriptorSetModifier dsModifier;
			dsModifier.UpdateImageBinding(updateInfo, &imageInfos);
			dsModifier.ModifySets(m_Device->handle);

			// Clear the builder for the next pipeline
			builder.Clear();
		}

		INDY_CORE_WARN("Building Graphics Pipeline...");
		// Build Graphics Pipeline
		{
			options.type = INDY_PIPELINE_TYPE_GRAPHICS;

			// Bind Shaders
			builder.BindShader("shaders/triangle.glsl.vert");
			builder.BindShader("shaders/triangle.glsl.frag");

			// Build and retrieve pipeline
			builder.Build(&options);
			m_GraphicsPipeline = builder.GetPipeline();

			// Clear the builder for the next pipeline
			builder.Clear();
		}

		// Build Raytrace Pipeline
		{

		}

		INDY_CORE_WARN("Done!");
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