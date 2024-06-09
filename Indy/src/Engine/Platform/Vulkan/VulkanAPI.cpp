
#include <Engine/Core/LogMacros.h>

#include <cstdint>
#include <memory>
#include <vector>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

import Indy.Application;
import Indy.Graphics;

import Indy.Profiler;

import Indy.VulkanGraphics;

import Indy.Events;

namespace Indy
{
	VulkanAPI::VulkanAPI()
		: m_Instance(VK_NULL_HANDLE), m_DebugMessenger(VK_NULL_HANDLE)
	{
		Events<WindowDispatchEvent>::Subscribe<VulkanAPI>(this, &VulkanAPI::OnWindowDispatch);
	}

	VulkanAPI::~VulkanAPI()
	{
		
	}

	// Application Phase Functions
	// ------------------------------------------------------------------

	void VulkanAPI::OnLoad()
	{
		if (!InitVulkan())
			INDY_CORE_CRITICAL("Failed to initialize Vulkan!");
	}

	void VulkanAPI::OnStart()
	{
		
	}

	void VulkanAPI::OnUnload()
	{
		Cleanup();
	}

	void VulkanAPI::OnWindowDispatch(WindowDispatchEvent* event)
	{
		m_TestRenderer = std::make_unique<VulkanRenderer>(event->window, m_Instance);

		Application::Get().OnUpdate.Subscribe([this]() { m_TestRenderer->Render(); });
	}

	// Internal Methods
	// -----------------------------------------------------------------

	bool VulkanAPI::InitVulkan()
	{
		if (!SupportsValidationLayers())
			return false;

		if (!CreateVulkanInstance())
			return false;

		// Retreive GPU information
		VulkanDevice::GetAllGPUSpecs(m_Instance);

		return true;
	}

	void VulkanAPI::Cleanup()
	{
#ifdef ENGINE_DEBUG

		INDY_CORE_TRACE("Destroying Vulkan Debug Messenger...");
		Vulkan_DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);

#endif

		INDY_CORE_TRACE("Destroying Vulkan Instance...");
		vkDestroyInstance(m_Instance, nullptr);
	}

	bool VulkanAPI::SupportsValidationLayers()
	{
		// Query for the number of instance layers
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		// Query for all layer properties
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		// Ensure all requested layers are available
		for (const char* validationLayer : g_Vulkan_Validation_Layers)
		{
			bool found = false;

			// Find validation layer in instance layers
			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(validationLayer, layerProperties.layerName) == 0)
				{
					found = true;
					break;
				}
			}

			// return false if this validation layer wasn't found
			if (!found)
			{
				INDY_CORE_CRITICAL(
					"[Vulkan Backend] One or more requested validation layers are not available!\nFirst is: {0}",
					validationLayer
				);
				return false;
			}
		}

		return true;
	}

	bool VulkanAPI::CreateVulkanInstance()
	{
		// Vulkan Application Info
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Indy Engine";
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 261);

		// Vulkan Instance Create Info
		VkInstanceCreateInfo instanceCreateInfo{};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &appInfo;

#ifdef ENGINE_DEBUG

		// Setup Debug Messenger
		VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
		debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugMessengerCreateInfo.pfnUserCallback = Vulkan_DebugCallback;
		debugMessengerCreateInfo.pUserData = nullptr;

		// Enable Validation Layers
		instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(g_Vulkan_Validation_Layers.size());
		instanceCreateInfo.ppEnabledLayerNames = g_Vulkan_Validation_Layers.data();

		// Attach debug messenger create info. This ONLY tracks messages from the instance
		instanceCreateInfo.pNext = &debugMessengerCreateInfo;

#else

		instanceCreateInfo.enabledLayerCount = 0;
		instanceCreateInfo.pNext = nullptr;

#endif

		// Retrieve Required Extensions for GLFW
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		if (glfwExtensionCount == 0 || !glfwExtensions)
		{
			INDY_CORE_CRITICAL("Could not initialize Vulkan API: GLFW is either unavailable or hasn't been initialized.");
			return false;
		}

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef ENGINE_DEBUG

		extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); // Enable vulkan debug utilities

#endif

		// Attach required extensions, if everything is valid
		instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

		// Create Vulkan Instance
		if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance) != VK_SUCCESS)
		{
			INDY_CORE_CRITICAL("Failed to create Vulkan instance.");
			return false;
		}

#ifdef ENGINE_DEBUG
		return CreateDebugMessenger(debugMessengerCreateInfo);
#endif

		return true;
	}

	bool VulkanAPI::CreateDebugMessenger(const VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
#ifdef ENGINE_DEBUG
		// For more general vulkan debugging, an explicit debug messenger must be created
		if (Vulkan_CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create debug messenger.");
			return false;
		}
#endif
		return true;
	}
}
