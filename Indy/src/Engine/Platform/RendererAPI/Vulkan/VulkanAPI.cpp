#include "VulkanAPI.h"

#include "Engine/Core/Log.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	VkInstance VulkanAPI::s_Vulkan_Instance;
	uint32_t VulkanAPI::MAX_FRAMES_IN_FLIGHT = 2;
	void* VulkanAPI::s_Window = nullptr;

	VulkanAPI::VulkanAPI() : m_WindowSurface(nullptr)
	{
		Events::Bind<VulkanAPI>("RenderContext", "WindowResize", this, &VulkanAPI::onWindowResize);

		Event event{ "LayerContext", "RequestWindow" };
		Events::Dispatch(event);
		s_Window = event.data;

	}

	VulkanAPI::~VulkanAPI()
	{
		Shutdown();
	}

	void VulkanAPI::Init()
	{
		VulkanDebugUtil::Init();

		// Instance Creation
		CreateInstance();

		// Setup the debug messenger
		VulkanDebugUtil::CreateMessenger(s_Vulkan_Instance);

		// Window Surface Creation
		CreateWindowSurface();

		// Initialize the physical and logical devices
		VulkanDevice::Init(s_Vulkan_Instance, m_WindowSurface);

		// Initialize the swap chain and create the image views
		VulkanSwapChain::Init(m_WindowSurface);
		VulkanSwapChain::CreateImageViews();

		// Initialize the graphics pipeline
		VulkanPipeline::Init();

		// Framebuffer creation
		VulkanSwapChain::CreateFramebuffers();

		// Initialize the command pool
		VulkanCommandPool::Init(MAX_FRAMES_IN_FLIGHT);

		// Create CPU <-> GPU Sync objects
		CreateSyncObjects();
	}

	void VulkanAPI::Shutdown()
	{
		const VkDevice& logicalDevice = VulkanDevice::GetLogicalDevice();

		VulkanSwapChain::Shutdown();
		VulkanPipeline::Shutdown();

		// Clean up frame semaphores and fences
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(logicalDevice, m_FramesInFlight[i].renderFinishedSemaphore, nullptr);
			vkDestroySemaphore(logicalDevice, m_FramesInFlight[i].imageAvailableSemaphore, nullptr);
			vkDestroyFence(logicalDevice, m_FramesInFlight[i].fence, nullptr);
		}

		VulkanCommandPool::Shutdown();
		VulkanDevice::Shutdown();

		VulkanDebugUtil::Shutdown(s_Vulkan_Instance);

		vkDestroySurfaceKHR(s_Vulkan_Instance, m_WindowSurface, nullptr);
		vkDestroyInstance(s_Vulkan_Instance, nullptr);
	}

	// ----------------------
	// Window Resize Handling
	// ----------------------

	void VulkanAPI::onWindowResize(Event& event)
	{
		if (m_FramebufferResized) return;

		m_FramebufferResized = true;

		// To enable the framebuffer to actually render during resizing, instead of after the resize
		//	is complete, two calls to drawFrame are necessary. GLFW halts the event loop while resizing
		//	is in progress, but this callback is fired for every mouse movement while resizing.
		//	The first call to DrawFrame() will only recreate the swap chain. A second call is needed to actually render
		//	the result.
		DrawFrame();
		DrawFrame();
	}

	void VulkanAPI::RecreateSwapChain()
	{
		const VkDevice& logicalDevice = VulkanDevice::GetLogicalDevice();
		std::vector<VkFramebuffer>& framebuffers = VulkanSwapChain::GetFrameBuffers();

		GLFWwindow* window = (GLFWwindow*)VulkanAPI::s_Window;

		// Window Minimization. Don't poll events while the window is inactive.
		int width = 0, height = 0;
		glfwGetFramebufferSize(window, &width, &height);
		while (width == 0 && height == 0)
		{
			glfwGetFramebufferSize(window, &width, &height);
			glfwWaitEvents();
		}
		
		vkDeviceWaitIdle(logicalDevice);

		// Destroy out of date swap chain
		VulkanSwapChain::Cleanup();

		// Re-initialize swap chain
		VulkanSwapChain::Init(m_WindowSurface);

		// Recreate image views
		VulkanSwapChain::CreateImageViews();

		// Recreate FrameBuffers
		VulkanSwapChain::CreateFramebuffers();
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
		if (VulkanDebugUtil::Enabled())
		{
			createInfo.enabledLayerCount = VulkanDebugUtil::GetValidationLayerCount();
			createInfo.ppEnabledLayerNames = VulkanDebugUtil::GetValidationLayerNames();

			VulkanDebugUtil::PopulateMessengerCreateInfo(debugCreateInfo);
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

		if (VulkanDebugUtil::Enabled()) {
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

	void VulkanAPI::CreateSyncObjects()
	{
		const VkDevice& logicalDevice = VulkanDevice::GetLogicalDevice();

		m_FramesInFlight.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &m_FramesInFlight[i].imageAvailableSemaphore) != VK_SUCCESS ||
				vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &m_FramesInFlight[i].renderFinishedSemaphore) != VK_SUCCESS ||
				vkCreateFence(logicalDevice, &fenceInfo, nullptr, &m_FramesInFlight[i].fence) != VK_SUCCESS) {
				INDY_CORE_ERROR("failed to create semaphores!");
			}
		}
	}

	void VulkanAPI::DrawFrame()
	{
		const VkDevice& logicalDevice = VulkanDevice::GetLogicalDevice();

		const VkSwapchainKHR& swapchain = VulkanSwapChain::GetSwapChain();
		std::vector<VkFramebuffer>& framebuffers = VulkanSwapChain::GetFrameBuffers();
		
		std::vector<VkCommandBuffer>& commandBuffers = VulkanCommandPool::GetCommandBuffers();

		// Wait for previous frame to finish
		vkWaitForFences(logicalDevice, 1, &m_FramesInFlight[m_CurrentFrame].fence, VK_TRUE, UINT64_MAX);

		// Get Image from Swap Chain
		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(logicalDevice, swapchain, UINT64_MAX, m_FramesInFlight[m_CurrentFrame].imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

		// Recreate Swap Chain if it is out of date, or suboptimal
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			INDY_CORE_CRITICAL("Failed to Acquire Swap Chain Image!");
		}

		// Reset the fence only if work was submitted
		vkResetFences(logicalDevice, 1, &m_FramesInFlight[m_CurrentFrame].fence);

		// Reset the command buffer and record the commands
		vkResetCommandBuffer(commandBuffers[m_CurrentFrame], 0);
		VulkanCommandPool::RecordCommandBuffer(commandBuffers[m_CurrentFrame], framebuffers, imageIndex);

		// Submit the command buffer to the correct queue
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		// Specify when and where to wait in the graphics pipeline
		VkSemaphore waitSemaphores[] = { m_FramesInFlight[m_CurrentFrame].imageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		// attach the command buffer
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[m_CurrentFrame];

		// Specify which semaphores to signal when the command buffers finish execution
		VkSemaphore signalSemaphores[] = { m_FramesInFlight[m_CurrentFrame].renderFinishedSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		// Submit commands to the graphics queue.
		const VulkanQueue& graphicsQueue = VulkanDevice::GetGraphicsQueue();
		if (vkQueueSubmit(graphicsQueue.queue, 1, &submitInfo, m_FramesInFlight[m_CurrentFrame].fence) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to submit draw command buffer!");
			return;
		}

		// Submit results back to the swap chain
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { swapchain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional

		const VulkanQueue& presentQueue = VulkanDevice::GetPresentQueue();
		result = vkQueuePresentKHR(presentQueue.queue, &presentInfo);

		// Recreate Swap Chain if it is out of date, or suboptimal
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_FramebufferResized)
		{
			m_FramebufferResized = false;
			RecreateSwapChain();
		}
		else if (result != VK_SUCCESS)
		{
			INDY_CORE_CRITICAL("Failed to Acquire Swap Chain Image!");
		}

		m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}
}

	
