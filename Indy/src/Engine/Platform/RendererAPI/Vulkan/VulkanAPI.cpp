#include "VulkanAPI.h"

#include "Engine/Core/Log.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	VkInstance VulkanAPI::s_Vulkan_Instance;

	VulkanAPI::VulkanAPI()
	{
		m_DebugUtil = std::make_unique<VulkanDebugUtil>();
		m_Device = std::make_unique<VulkanDevice>();
		m_SwapChain = std::make_unique<VulkanSwapChain>();
		m_Pipeline = std::make_unique<VulkanPipeline>();
		m_CommandPool = std::make_unique<VulkanCommandPool>();
	}

	VulkanAPI::~VulkanAPI()
	{
		Shutdown();
	}

	void VulkanAPI::Init()
	{
		// Ensure validation layers are supported
		m_DebugUtil->QueryValidationLayerSupport();

		// Instance Creation
		CreateInstance();

		// Setup the debug messenger, if enabled
		m_DebugUtil->CreateDebugMessenger(s_Vulkan_Instance);

		// Window Surface Creation
		CreateWindowSurface();

		// Initialize the physical and logical devices
		m_Device->Init(s_Vulkan_Instance, m_WindowSurface);

		// Initialize the swap chain and create the image views
		m_SwapChain->Init(m_WindowSurface);
		m_SwapChain->CreateImageViews();

		// Initialize the graphics pipeline
		m_Pipeline->Init();

		// Framebuffer creation
		CreateFramebuffers();

		// Initialize the command pool
		m_CommandPool->Init();

		// Create CPU <-> GPU Sync objects
		CreateSyncObjects();

		// Draw Test Frame
		DrawFrame();
	}

	void VulkanAPI::Shutdown()
	{
		const VulkanDeviceInfo& deviceInfo = VulkanDevice::GetDeviceInfo();

		vkDestroySemaphore(deviceInfo.logicalDevice, m_ImageAvailableSemaphore, nullptr);
		vkDestroySemaphore(deviceInfo.logicalDevice, m_RenderFinishedSemaphore, nullptr);
		vkDestroyFence(deviceInfo.logicalDevice, m_InFlightFence, nullptr);

		m_CommandPool->Shutdown();

		for (auto framebuffer : m_Framebuffers)
		{
			vkDestroyFramebuffer(deviceInfo.logicalDevice, framebuffer, nullptr);
		}

		m_Pipeline->Shutdown();

		m_SwapChain->Shutdown();
		m_Device->Shutdown();
		m_DebugUtil->DestroyDebugMessenger(s_Vulkan_Instance);

		vkDestroySurfaceKHR(s_Vulkan_Instance, m_WindowSurface, nullptr);
		vkDestroyInstance(s_Vulkan_Instance, nullptr);
	}

	void VulkanAPI::onApplicationClose(Event& event)
	{
		Shutdown();
	}

	// ------------------------
	// Vulkan Instance Creation
	// ------------------------

	void VulkanAPI::CreateInstance()
	{
		// App Info
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Indy Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		// Instance Creation Info
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		// Getting Platform Specific Extensions
		auto extensions = GetRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (m_DebugUtil->Enabled())
		{
			createInfo.enabledLayerCount = m_DebugUtil->GetValidationLayerCount();
			createInfo.ppEnabledLayerNames = m_DebugUtil->GetValidationLayerNames();

			m_DebugUtil->PopulateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else
		{
			createInfo.enabledLayerCount = 0;

			createInfo.pNext = nullptr;
		}

		if (vkCreateInstance(&createInfo, nullptr, &s_Vulkan_Instance) != VK_SUCCESS) {
			INDY_CORE_ERROR("failed to create instance!");
		}
	}

	std::vector<const char*> VulkanAPI::GetRequiredExtensions()
	{
		// GLFW extensions are always required.
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (m_DebugUtil->Enabled()) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	// -----------------------
	// Window Surface Creation
	// -----------------------

	void VulkanAPI::CreateWindowSurface()
	{
		// Get Window
		Event event{"LayerContext", "RequestWindow"};
		Events::Dispatch(event);
		GLFWwindow* GLFW_Window = (GLFWwindow*)event.data;

		if (glfwCreateWindowSurface(s_Vulkan_Instance, GLFW_Window, nullptr, &m_WindowSurface) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create window surface!");
		}
	}

	// --------------------
	// Framebuffer Creation
	// --------------------

	void VulkanAPI::CreateFramebuffers()
	{
		const VulkanDeviceInfo& deviceInfo = VulkanDevice::GetDeviceInfo();
		const VulkanSwapchainInfo& swapchainInfo = VulkanSwapChain::GetSwapChainInfo();
		const VulkanPipelineInfo& pipelineInfo = VulkanPipeline::GetPipelineInfo();

		m_Framebuffers.resize(swapchainInfo.imageViews.size());

		for (size_t i = 0; i < swapchainInfo.imageViews.size(); i++)
		{
			VkImageView attachments[] = {
				swapchainInfo.imageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = pipelineInfo.renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = swapchainInfo.extent.width;
			framebufferInfo.height = swapchainInfo.extent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(deviceInfo.logicalDevice, &framebufferInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS)
			{
				INDY_CORE_ERROR("[VulkanAPI] Failed to create framebuffer!");
			}
		}
	}

	void VulkanAPI::CreateSyncObjects()
	{
		const VulkanDeviceInfo& deviceInfo = VulkanDevice::GetDeviceInfo();

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		if (vkCreateSemaphore(deviceInfo.logicalDevice, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphore) != VK_SUCCESS ||
			vkCreateSemaphore(deviceInfo.logicalDevice, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore) != VK_SUCCESS ||
			vkCreateFence(deviceInfo.logicalDevice, &fenceInfo, nullptr, &m_InFlightFence) != VK_SUCCESS) {
			INDY_CORE_ERROR("failed to create semaphores!");
		}
	}

	void VulkanAPI::DrawFrame()
	{
		const VulkanDeviceInfo& deviceInfo = VulkanDevice::GetDeviceInfo();
		const VulkanSwapchainInfo& swapchainInfo = VulkanSwapChain::GetSwapChainInfo();
		const VulkanCommandPoolInfo& commandPoolInfo = VulkanCommandPool::GetCommandPoolInfo();

		// Wait for previous frame to finish
		vkWaitForFences(deviceInfo.logicalDevice, 1, &m_InFlightFence, VK_TRUE, UINT64_MAX);
		vkResetFences(deviceInfo.logicalDevice, 1, &m_InFlightFence);

		// Get Image from Swap Chain
		uint32_t imageIndex;
		vkAcquireNextImageKHR(deviceInfo.logicalDevice, swapchainInfo.swapchain, UINT64_MAX, m_ImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

		// Reset the command buffer and record the commands
		vkResetCommandBuffer(commandPoolInfo.commandBuffer, 0);
		m_CommandPool->RecordCommandBuffer(commandPoolInfo.commandBuffer, m_Framebuffers, imageIndex);

		// Submit the command buffer to the correct queue
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		// Specify when and where to wait in the graphics pipeline
		VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		// attach the command buffer
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandPoolInfo.commandBuffer;

		// Specify which semaphores to signal when the command buffers finish
		VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		// Submit commands to the graphics queue.
		if (vkQueueSubmit(deviceInfo.graphicsQueue, 1, &submitInfo, m_InFlightFence) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to submit draw command buffer!");
			return;
		}

		// Submit results back to the swap chain
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { swapchainInfo.swapchain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional

		vkQueuePresentKHR(deviceInfo.presentQueue, &presentInfo);
	}
}

	
