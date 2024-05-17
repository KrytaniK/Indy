
#include <Engine/Core/LogMacros.h>

#include <cstdint>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

import Indy.VulkanRenderer;

namespace Indy
{
	VulkanBackend::VulkanBackend()
	{
		// Initialize Vulkan if it hasn't already been initialized
		if (VulkanBackend::s_ReferenceCount == 0)
		{
			INDY_CORE_INFO("Attempting to initialize Vulkan...");
			if (VulkanBackend::Init())
			{
				INDY_CORE_INFO("Vulkan Successfully Initialized!");
				VulkanBackend::s_ReferenceCount++;
				return;
			}

			INDY_CORE_CRITICAL("Failed to initialized Vulkan!");

			return;
		}

		VulkanBackend::s_ReferenceCount++;
		INDY_CORE_INFO("Vulkan Reference Count: {0}", VulkanBackend::s_ReferenceCount);
	}

	VulkanBackend::~VulkanBackend()
	{
		if (--VulkanBackend::s_ReferenceCount != 0)
			return;

		INDY_CORE_WARN("Starting Vulkan cleanup process... \n\tRemaining References: {0}", VulkanBackend::s_ReferenceCount);

		VulkanBackend::Cleanup();
	}

	VkInstance* VulkanBackend::GetInstance()
	{
		return &VulkanBackend::s_SharedResources.instance;
	}

	VulkanSharedResources* VulkanBackend::GetSharedResources()
	{
		return &VulkanBackend::s_SharedResources;
	}

	bool VulkanBackend::Init()
	{
		INDY_CORE_TRACE("Checking for validation layer support...");

		// Ensure Validation Layer Support
		if (!VulkanBackend::SupportsValidationLayers())
			return false;

		INDY_CORE_TRACE("Initializing vulkan instance structures...");

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

		INDY_CORE_TRACE("Setting up a debug messenger...");

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
		instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugMessengerCreateInfo;

#else

		INDY_CORE_INFO("Debug Mode Disabled!");

		instanceCreateInfo.enabledLayerCount = 0;
		instanceCreateInfo.pNext = nullptr;

#endif

		INDY_CORE_TRACE("Ensuring GLFW extensions are present...");
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
		INDY_CORE_TRACE("Adding Debug util extension...");
		extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); // Enable vulkan debug utilities
#endif

		// Attach required extensions, if everything is valid
		instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

		INDY_CORE_TRACE("Creating Instance...");

		// Create Vulkan Instance
		if (vkCreateInstance(&instanceCreateInfo, nullptr, &VulkanBackend::s_SharedResources.instance) != VK_SUCCESS)
		{
			INDY_CORE_CRITICAL("Failed to create Vulkan instance.");
			return false;
		}

#ifdef ENGINE_DEBUG

		INDY_CORE_TRACE("Creating Debug Messenger...");
		// For more general vulkan debugging, an explicit debug messenger must be created
		if (Vulkan_CreateDebugUtilsMessengerEXT(VulkanBackend::s_SharedResources.instance, &debugMessengerCreateInfo, nullptr, &VulkanBackend::s_DebugMessenger) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create debug messenger.");
			return false;
		}

#endif

		INDY_CORE_TRACE("Retrieving GPU information...");

		// Store all physical devices and their information
		VulkanBackend::s_SharedResources.devices = VulkanDevice::GetAllPhysicalDevices(VulkanBackend::s_SharedResources.instance);

		// Need a primary physical device to interface with
		VulkanDeviceCompatibility deviceCompatibility;
		deviceCompatibility.graphics = true;									// Graphics operations are necessary
		//deviceCompatibility.present = true;										// Presentation is necessary
		deviceCompatibility.compute = true;										// Compute operations should be supported
		deviceCompatibility.geometryShader = true;
		deviceCompatibility.deviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;	// Prefer dedicated GPUs

		INDY_CORE_TRACE("Checking for an dedicated GPU...");

		// Check for desired compatibility
		for (auto& physicalDevice : VulkanBackend::s_SharedResources.devices)
		{
			if (VulkanDevice::IsDeviceCompatible(physicalDevice, deviceCompatibility))
				VulkanBackend::s_SharedResources.physicalDevice = physicalDevice.device;
		}

		// If a GPU wasn't found with the desired capability
		if (VulkanBackend::s_SharedResources.physicalDevice == VK_NULL_HANDLE)
		{
			INDY_CORE_WARN("Could not find a dedicated GPU suitable for Vulkan operations!");

			INDY_CORE_TRACE("Checking for a compatible GPU...");

			// Do another compatibility check, this time searching for ANY gpu that supports
			// graphics and present queues.
			deviceCompatibility.deviceType = VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM;
			for (auto& physicalDevice : VulkanBackend::s_SharedResources.devices)
			{
				if (VulkanDevice::IsDeviceCompatible(physicalDevice, deviceCompatibility))
					VulkanBackend::s_SharedResources.physicalDevice = physicalDevice.device;
			}

			// If a compatible GPU still wasn't found, it's safe to assume something is
			// either very wrong, or vulkan isn't supported.
			if (VulkanBackend::s_SharedResources.physicalDevice == VK_NULL_HANDLE)
			{
				INDY_CORE_CRITICAL("Could not find a compatible GPU!");
				return false;
			}
		}

		INDY_CORE_INFO("Compatible GPU found!");

		return true;
	}

	void VulkanBackend::Cleanup()
	{
#ifdef ENGINE_DEBUG

		INDY_CORE_TRACE("Destroying Vulkan Debug Messenger...");
		Vulkan_DestroyDebugUtilsMessengerEXT(VulkanBackend::s_SharedResources.instance, VulkanBackend::s_DebugMessenger, nullptr);

#endif

		// Destroy Logical devices before instance

		INDY_CORE_TRACE("Destroying Vulkan Instance...");
		vkDestroyInstance(VulkanBackend::s_SharedResources.instance, nullptr);
	}

	bool VulkanBackend::SupportsValidationLayers()
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

	// --------------------------------
	// Vulkan Debug Messenger Functions
	// --------------------------------

	VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan_DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			INDY_CORE_ERROR("[Vulkan Debug] {0}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			INDY_CORE_WARN("[Vulkan Debug] {0}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			INDY_CORE_TRACE("[Vulkan Debug] {0}", pCallbackData->pMessage);
			break;
		default:
			return VK_FALSE;
			// INDY_CORE_INFO("[Vulkan Debug] {0}", pCallbackData->pMessage);
		}

		return VK_FALSE;
	}

	VkResult Vulkan_CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		// Retrieve debug messenger create function from Vulkan
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

		// Execute function
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			INDY_CORE_ERROR("Extension Not Present!");
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void Vulkan_DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
		// Retrieve debug messenger destroy function from Vulkan
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

		// Execute function
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}

}