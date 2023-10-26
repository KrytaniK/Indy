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
	}

	VulkanAPI::~VulkanAPI()
	{
		Shutdown();
	}

	void VulkanAPI::Init()
	{
		m_DebugUtil->QueryValidationLayerSupport();

		CreateInstance();

		m_DebugUtil->CreateDebugMessenger(s_Vulkan_Instance);

		CreateWindowSurface();

		m_Device->Init(s_Vulkan_Instance, m_WindowSurface);

		auto swapChainSupportDetails = m_Device->GetSwapChainSupportDetails();

		m_SwapChain->Init(swapChainSupportDetails, m_WindowSurface);
		m_SwapChain->CreateImageViews();
	}

	void VulkanAPI::Shutdown()
	{
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
}

	
