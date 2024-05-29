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
	VulkanRenderTarget::VulkanRenderTarget(const VkInstance& instance, const GPUCompatibility& compatibility, Window* window)
	{
		m_ID = window->Properties().id;
		m_Instance = instance;

		if (!window)
			return;

		// Create VkSurface
		if (glfwCreateWindowSurface(m_Instance, static_cast<GLFWwindow*>(window->NativeWindow()), nullptr, &m_Surface) != VK_SUCCESS)
		{
			INDY_CORE_CRITICAL("Failed to create window surface!");
			return;
		}

		// Create Logical Device with surface support
		m_Device = std::make_unique<VulkanDevice>(compatibility, m_Surface);
		auto physicalDevice = m_Device->GetPhysicalDevice();

		// Create Swapchain with window
		m_Swapchain = std::make_unique<VulkanSwapchain>(physicalDevice, m_Device->Get(), m_Surface, window);

		// Allocate frame data for this render target
		m_Frames.reserve(g_Max_Frames_In_Flight);
		for (uint8_t i = 0; i < g_Max_Frames_In_Flight; i++)
			m_Frames.emplace_back(std::make_unique<VulkanFrame>(m_Device->Get(), physicalDevice->queueFamilies));

		m_FrameCount = 0;
	}

	VulkanRenderTarget::~VulkanRenderTarget()
	{
		m_Swapchain = nullptr; // explicitly destroy swap chain BEFORE VkSurface.

		if (&m_Surface)
			vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	}

	void VulkanRenderTarget::Draw()
	{
		// Get Current Frame
		auto frame = GetCurrentFrame();

		auto commandPool = frame->GetCommandPool();
		uint8_t commandBufferIndex = 0;
		auto& commandBuffer = commandPool->GetCommandBuffer(commandBufferIndex);

		auto& renderFence = frame->GetFence("Render");
		auto& renderSemaphore = frame->GetSemaphore("Render");
		auto& swapchainSemaphore = frame->GetSemaphore("Swapchain");

		if (vkWaitForFences(m_Device->Get(), 1, &renderFence, VK_TRUE, UINT64_MAX) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Error waiting for Render Fence");
			return;
		}

		if (vkResetFences(m_Device->Get(), 1, &renderFence) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Error resetting Render Fence");
			return;
		}

		uint32_t swapchainImageIndex;
		if (vkAcquireNextImageKHR(m_Device->Get(), m_Swapchain->Get(), UINT64_MAX, swapchainSemaphore, nullptr, &swapchainImageIndex) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Error retrieving next swapchain image!");
			return;
		}

		// Begin the command buffer
		commandPool->BeginCommandBuffer(commandBufferIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		// Transition swapchain image for writing
		m_Swapchain->TransitionImage(commandBuffer, swapchainImageIndex, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

		// Clear Swap chain image
		m_Swapchain->ClearImage(commandBuffer, m_FrameCount, swapchainImageIndex, VK_IMAGE_LAYOUT_GENERAL);

		// Transition swapchain image into present mode
		m_Swapchain->TransitionImage(commandBuffer, swapchainImageIndex, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		// Finalize command buffer
		commandPool->EndCommandBuffer(commandBufferIndex);

		// Prepare for presentation
		VkSubmitInfo2 submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
		submitInfo.pNext = nullptr;

		// Semaphores
		VkSemaphoreSubmitInfo swapchainSemaphoreSubmitInfo{};
		swapchainSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		swapchainSemaphoreSubmitInfo.pNext = nullptr;
		swapchainSemaphoreSubmitInfo.semaphore = swapchainSemaphore;
		swapchainSemaphoreSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR;
		swapchainSemaphoreSubmitInfo.deviceIndex = 0;
		swapchainSemaphoreSubmitInfo.value = 1;

		VkSemaphoreSubmitInfo renderSemaphoreSubmitInfo{};
		renderSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		renderSemaphoreSubmitInfo.pNext = nullptr;
		renderSemaphoreSubmitInfo.semaphore = renderSemaphore;
		renderSemaphoreSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
		renderSemaphoreSubmitInfo.deviceIndex = 0;
		renderSemaphoreSubmitInfo.value = 1;

		// Command buffer
		VkCommandBufferSubmitInfo cmdBufferSubmitInfo{};
		cmdBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
		cmdBufferSubmitInfo.pNext = nullptr;
		cmdBufferSubmitInfo.commandBuffer = commandBuffer;
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
		if (vkQueueSubmit2(m_Device->Queues()->GetGraphicsQueue(), 1, &submitInfo, renderFence) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Error Submitting Graphics Queue!");
			return;
		}

		// Present image
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.pSwapchains = &m_Swapchain->Get();
		presentInfo.swapchainCount = 1;

		// Attach wait semaphore
		presentInfo.pWaitSemaphores = &renderSemaphore;
		presentInfo.waitSemaphoreCount = 1;

		// Attach swap chain image index
		presentInfo.pImageIndices = &swapchainImageIndex;

		if (vkQueuePresentKHR(m_Device->Queues()->GetGraphicsQueue(), &presentInfo) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Error Submitting Graphics Queue!");
			return;
		}

		m_FrameCount++;
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
