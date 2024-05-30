#include "Engine/Core/LogMacros.h"

#include <memory>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

import Indy.Profiler;
import Indy.Graphics;
import Indy.VulkanGraphics;
import Indy.Events;

namespace Indy
{
	VulkanRenderTarget::VulkanRenderTarget(const VkInstance& instance, const VulkanRTSpec& spec)
	{
		m_ID = spec.window->Properties().id;
		m_Instance = instance;
		m_DeviceHandle = spec.deviceHandle;
		m_ComputePipeline = spec.computePipeline;
		m_RenderImageDescriptor = spec.imageDescriptor;
		m_FrameCount = 0;

		if (!spec.useSurface)
			return;

		// Create VkSurface
		if (glfwCreateWindowSurface(m_Instance, static_cast<GLFWwindow*>(spec.window->NativeWindow()), nullptr, &m_Surface) != VK_SUCCESS)
		{
			INDY_CORE_CRITICAL("Failed to create window surface!");
			return;
		}

		auto physicalDevice = m_DeviceHandle->GetPhysicalDevice();
		if (!VulkanDevice::GetGPUSurfaceSupport(physicalDevice, m_Surface))
		{
			INDY_CORE_ERROR("Failed to create render target. GPU not support surface presentation!");
			return;
		}

		// Create Swapchain with window
		m_Swapchain = std::make_unique<VulkanSwapchain>(physicalDevice, m_DeviceHandle->Get(), m_Surface, spec.window);

		// Allocate frame data for this render target
		m_Frames.reserve(g_Max_Frames_In_Flight);
		for (uint8_t i = 0; i < g_Max_Frames_In_Flight; i++)
			m_Frames.emplace_back(std::make_unique<VulkanFrame>(m_DeviceHandle->Get(), physicalDevice->queueFamilies));

		// Allocate the image we want to render to
		VulkanImageSpec renderImageSpec{};
		renderImageSpec.extent = {
			spec.window->Properties().width,
			spec.window->Properties().height,
			1
		};
		renderImageSpec.format = VK_FORMAT_R16G16B16A16_SFLOAT;
		renderImageSpec.usageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		renderImageSpec.usageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		renderImageSpec.usageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
		renderImageSpec.usageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		renderImageSpec.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;

		m_RenderImage = std::make_unique<VulkanImage>(m_DeviceHandle->GetVmaAllocator(), m_DeviceHandle->Get(), renderImageSpec);

		// Initialize Descriptor Sets
		VkDescriptorImageInfo imgInfo{};
		imgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imgInfo.imageView = m_RenderImage->GetView();

		VkWriteDescriptorSet renderImageWrite = {};
		renderImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		renderImageWrite.pNext = nullptr;

		// Bind Compute shader descriptor set (binding = 0)
		renderImageWrite.dstBinding = 0;
		renderImageWrite.dstSet = spec.imageDescriptor->GetSet();
		renderImageWrite.descriptorCount = 1;
		renderImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		renderImageWrite.pImageInfo = &imgInfo;

		vkUpdateDescriptorSets(m_DeviceHandle->Get(), 1, &renderImageWrite, 0, nullptr);
	}

	VulkanRenderTarget::~VulkanRenderTarget()
	{
		// Wait for GPU to finish up any tasks
		vkDeviceWaitIdle(m_DeviceHandle->Get());

		// Explicitly delete all frames
		for (int i = 0; i < m_Frames.size(); i++)
			m_Frames[i] = nullptr;

		// Explicitly delete render image
		m_RenderImage = nullptr;

		// Explicitly destroy swap chain.
		m_Swapchain = nullptr; 

		// Explicitly destroy surface
		if (&m_Surface)
			vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	}

	void VulkanRenderTarget::Render()
	{
		// Get Current Frame
		auto frame = GetCurrentFrame();

		// Begin Frame and retrieve current swap chain image index
		uint32_t swapchainImageIndex;
		OnBeginFrame(frame, swapchainImageIndex);

		OnDrawFrame(frame, swapchainImageIndex);

		OnEndFrame(frame);

		OnPresentFrame(frame, swapchainImageIndex);

		m_FrameCount++;
	}

	void VulkanRenderTarget::OnBeginFrame(VulkanFrame* frame, uint32_t& swapchainImageIndex)
	{
		if (vkWaitForFences(m_DeviceHandle->Get(), 1, &frame->GetFence("Render"), VK_TRUE, UINT64_MAX) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Error waiting for Render Fence");
			return;
		}

		if (vkResetFences(m_DeviceHandle->Get(), 1, &frame->GetFence("Render")) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Error resetting Render Fence");
			return;
		}

		if (vkAcquireNextImageKHR(m_DeviceHandle->Get(), m_Swapchain->Get(), UINT64_MAX, frame->GetSemaphore("Swapchain"), nullptr, &swapchainImageIndex) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Error retrieving next swapchain image!");
			return;
		}

		// Begin the command buffer (index 0 for the primary commmand buffer)
		frame->GetCommandPool()->BeginCommandBuffer(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	}

	void VulkanRenderTarget::OnDrawFrame(VulkanFrame* frame, const uint32_t& swapchainImageIndex)
	{
		// Get primary command buffer
		auto& commandBuffer = frame->GetCommandBuffer(0);

		// Transition render image to general layout so we can write to it
		VulkanImage::TransitionLayout(
			commandBuffer, m_RenderImage->Get(), 
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL
		);

		{ // Render Commands

			// Bind compute pipeline
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_ComputePipeline->Get());

			// Bind compute render image descriptors
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_ComputePipeline->GetLayout(), 0, 1, &m_RenderImageDescriptor->GetSet(), 0, nullptr);

			vkCmdDispatch(
				commandBuffer, 
				std::ceil(m_RenderImage->GetExtent().width / 16.0), 
				std::ceil(m_RenderImage->GetExtent().height / 16.0), 
				1
			);
		}

		// Transition render image to transfer source
		VulkanImage::TransitionLayout(
			commandBuffer, m_RenderImage->Get(), 
			VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
		);

		// Transition swapchain image to transfer destination
		VulkanImage::TransitionLayout(
			commandBuffer, m_Swapchain->GetImage(swapchainImageIndex).image, 
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
		);

		// Copy rendered image to swap chain image
		VulkanImage::Copy(
			commandBuffer, m_RenderImage->Get(), m_Swapchain->GetImage(swapchainImageIndex).image,
			{ m_RenderImage->GetExtent().width, m_RenderImage->GetExtent().height }, m_Swapchain->GetExtent()
		);

		// Transition swapchain image to presentation
		VulkanImage::TransitionLayout(
			commandBuffer, m_Swapchain->GetImage(swapchainImageIndex).image, 
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		);
	}

	void VulkanRenderTarget::OnEndFrame(VulkanFrame* frame)
	{
		// Finalize command buffer
		frame->GetCommandPool()->EndCommandBuffer(0);

		// Prepare for presentation
		VkSubmitInfo2 submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
		submitInfo.pNext = nullptr;

		// Semaphores
		VkSemaphoreSubmitInfo swapchainSemaphoreSubmitInfo{};
		swapchainSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		swapchainSemaphoreSubmitInfo.pNext = nullptr;
		swapchainSemaphoreSubmitInfo.semaphore = frame->GetSemaphore("Swapchain");
		swapchainSemaphoreSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR;
		swapchainSemaphoreSubmitInfo.deviceIndex = 0;
		swapchainSemaphoreSubmitInfo.value = 1;

		VkSemaphoreSubmitInfo renderSemaphoreSubmitInfo{};
		renderSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		renderSemaphoreSubmitInfo.pNext = nullptr;
		renderSemaphoreSubmitInfo.semaphore = frame->GetSemaphore("Render");
		renderSemaphoreSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
		renderSemaphoreSubmitInfo.deviceIndex = 0;
		renderSemaphoreSubmitInfo.value = 1;

		// Command buffer
		VkCommandBufferSubmitInfo cmdBufferSubmitInfo{};
		cmdBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
		cmdBufferSubmitInfo.pNext = nullptr;
		cmdBufferSubmitInfo.commandBuffer = frame->GetCommandBuffer(0);
		cmdBufferSubmitInfo.deviceMask = 0;

		// Attach wait(swapchain) and signal(render) semaphore submit infos
		submitInfo.waitSemaphoreInfoCount = 1;
		submitInfo.pWaitSemaphoreInfos = &swapchainSemaphoreSubmitInfo;
		submitInfo.signalSemaphoreInfoCount = 1;
		submitInfo.pSignalSemaphoreInfos = &renderSemaphoreSubmitInfo;

		// Attach command buffer submit info
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &cmdBufferSubmitInfo;

		// Queue up graphics operations
		if (vkQueueSubmit2(m_DeviceHandle->Queues()->GetGraphicsQueue(), 1, &submitInfo, frame->GetFence("Render")) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Error Submitting Graphics Queue!");
			return;
		}
	}

	void VulkanRenderTarget::OnPresentFrame(VulkanFrame* frame, const uint32_t& swapchainImageIndex)
	{
		// Present image
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.pSwapchains = &m_Swapchain->Get();
		presentInfo.swapchainCount = 1;

		// Attach wait semaphore
		presentInfo.pWaitSemaphores = &frame->GetSemaphore("Render");
		presentInfo.waitSemaphoreCount = 1;

		// Attach swap chain image index
		presentInfo.pImageIndices = &swapchainImageIndex;

		if (vkQueuePresentKHR(m_DeviceHandle->Queues()->GetGraphicsQueue(), &presentInfo) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Error Submitting Graphics Queue!");
			return;
		}
	}

	VulkanFrame* VulkanRenderTarget::GetCurrentFrame()
	{
		uint8_t frameIndex = m_FrameCount % g_Max_Frames_In_Flight;

		if (frameIndex == 0)
		{
			// Reset all command buffers once we've cycled through each frame
			m_Frames[frameIndex]->GetCommandPool()->Reset();
		}

		return m_Frames[frameIndex].get();
	}
}
