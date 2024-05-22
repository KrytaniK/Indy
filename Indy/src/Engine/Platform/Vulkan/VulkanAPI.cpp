
#include <Engine/Core/LogMacros.h>

#include <cstdint>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

import Indy.Application;
import Indy.VulkanGraphics;

import Indy.Events;

namespace Indy::Graphics
{
	VulkanAPI::VulkanAPI()
		: m_Instance(VK_NULL_HANDLE), m_DebugMessenger(VK_NULL_HANDLE)
	{
		Events<VulkanGPUEvent>::Subscribe<VulkanAPI>(this, &VulkanAPI::OnChoosePhysicalDevice);
	}

	VulkanAPI::~VulkanAPI()
	{
		
	}

	// Application Phase Functions
	// ------------------------------------------------------------------

	void VulkanAPI::OnLoad()
	{
		if (!Init())
			INDY_CORE_CRITICAL("Failed to initialize Vulkan!");
	}

	void VulkanAPI::OnStart()
	{

	}

	void VulkanAPI::OnUnload()
	{
		Cleanup();
	}

	// Event Handlers
	// ------------------------------------------------------------------

	void VulkanAPI::OnChoosePhysicalDevice(VulkanGPUEvent* event)
	{
		event->outDevice = GetCompatibleDevice(*event->compatibility);
	}

	// Internal Methods
	// ------------------------------------------------------------------

	bool VulkanAPI::Init()
	{
		INDY_CORE_TRACE("Checking for validation layer support...");

		// Ensure Validation Layer Support
		if (!SupportsValidationLayers())
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
		instanceCreateInfo.pNext = &debugMessengerCreateInfo;

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
		if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance) != VK_SUCCESS)
		{
			INDY_CORE_CRITICAL("Failed to create Vulkan instance.");
			return false;
		}

#ifdef ENGINE_DEBUG

		INDY_CORE_TRACE("Creating Debug Messenger...");
		// For more general vulkan debugging, an explicit debug messenger must be created
		if (Vulkan_CreateDebugUtilsMessengerEXT(m_Instance, &debugMessengerCreateInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create debug messenger.");
			return false;
		}

#endif

		INDY_CORE_TRACE("Retrieving GPU information...");

		// Store ALL physical devices and their information
		m_PhysicalDevices = GetAllPhysicalDevices(m_Instance);

		return true;
	}

	void VulkanAPI::Cleanup()
	{
#ifdef ENGINE_DEBUG

		INDY_CORE_TRACE("Destroying Vulkan Debug Messenger...");
		Vulkan_DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);

#endif

		// Destroy Logical devices before instance

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

	std::vector<std::shared_ptr<VulkanPhysicalDevice>> VulkanAPI::GetAllPhysicalDevices(const VkInstance& instance)
	{
		// Query physical device count
		uint32_t deviceCount = 0;
		if (vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to enumerate physical devices...");
			return {};
		}

		if (deviceCount == 0)
		{
			INDY_CORE_ERROR("Could not find any GPUs with Vulkan support!");
			return {};
		}

		// Create wrapper device vector and vulkan device vector
		std::vector<std::shared_ptr<VulkanPhysicalDevice>> pDevices(deviceCount);
		std::vector<VkPhysicalDevice> devices(deviceCount);

		INDY_CORE_TRACE("Enumerating Physical Devices...");

		// Retrieve all physical devices
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		INDY_CORE_TRACE("Physical Device Count: {0}", deviceCount);

		int i = 0;
		for (VkPhysicalDevice device : devices)
		{
			pDevices[i] = std::make_unique<VulkanPhysicalDevice>();

			// Copy each physical device into the wrapper vector
			pDevices[i]->handle = device;

			// Copy all device properties and features into the wrapper vector
			vkGetPhysicalDeviceProperties(device, &pDevices[i]->properties);
			vkGetPhysicalDeviceFeatures(device, &pDevices[i]->features);

			// Query for supported queue families
			uint32_t familyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);

			// Get all queue families
			std::vector<VkQueueFamilyProperties> queueFamProps(familyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, queueFamProps.data());

			// Query queue family support for each device
			int j = 0;
			for (const VkQueueFamilyProperties& props : queueFamProps)
			{
				// Check for support with graphics operations
				if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT)
					pDevices[i]->queueFamilies.graphics = j;

				// TODO: Implement presentation queue support check

				// Check for support with compute operations
				if (props.queueFlags & VK_QUEUE_COMPUTE_BIT)
					pDevices[i]->queueFamilies.compute = j;

				if (pDevices[i]->queueFamilies.Complete())
					break;

				j++;
			}

			INDY_CORE_INFO(
				"\n\t[{0}]\tID: {1}\tType: {2}\n\t\tSupports Graphics: {3}\n\t\tSupports Present: {4}\n\t\tSupports Compute: {5}\n\t\t",
				pDevices[i]->properties.deviceName, 
				pDevices[i]->properties.deviceID, 
				pDevices[i]->properties.deviceType,
				pDevices[i]->queueFamilies.graphics.has_value(),
				pDevices[i]->queueFamilies.present.has_value(),
				pDevices[i]->queueFamilies.compute.has_value()
			);

			i++;
		}

		// return the wrapper vector
		return pDevices;
	}

	std::shared_ptr<VulkanPhysicalDevice> VulkanAPI::GetCompatibleDevice(const VulkanDeviceCompatibility& compatibility)
	{
		std::shared_ptr<VulkanPhysicalDevice> outDevice = nullptr;

		uint8_t highestRating = 0;
		for (auto& device : m_PhysicalDevices)
		{
			uint8_t rating = RateDeviceCompatibility(*device, compatibility);
			if (rating != 0 && highestRating < rating)
			{
				outDevice = device;
				highestRating = rating;
			}
		}

		return outDevice;
	}

	uint8_t VulkanAPI::RateDeviceCompatibility(const VulkanPhysicalDevice& device, const VulkanDeviceCompatibility& compatibility)
	{
		
		uint8_t rating = 0;

		if (compatibility.type != VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM)
		{
			if (compatibility.type != device.properties.deviceType)
				return 0;

			rating += 10;
		}

		if (!IsCompatibleFeature(
			compatibility.graphics, 
			device.queueFamilies.graphics.has_value(), 
			rating)
			) return 0;

		if (!IsCompatibleFeature(
			compatibility.present, 
			device.queueFamilies.present.has_value(),
			rating)
			) return 0;

		if (!IsCompatibleFeature(
			compatibility.compute, 
			device.queueFamilies.compute.has_value(),
			rating)
			) return 0;

		if (!IsCompatibleFeature(
			compatibility.geometryShader, 
			device.features.geometryShader,
			rating)
			) return 0;


		INDY_CORE_TRACE("Device compatibility rating for [{0}]: {1}.", device.properties.deviceName, rating);
		return rating;
	}

	bool VulkanAPI::IsCompatibleFeature(const VulkanCompatibility& preference, bool hasFeature, uint8_t& rating)
	{
		switch (preference)
		{
			case VULKAN_COMPATIBILITY_VOID: return true;
			case VULKAN_COMPATIBILITY_PREFER:
			{
				if (hasFeature)
					rating += 10;
			};
			case VULKAN_COMPATIBILITY_REQUIRED:
			{
				if (hasFeature)
					rating += 10;
				else
					return false;
			}
		}

		return true;
	}
}
