#include <Engine/Core/LogMacros.h>

#include <cstdint>
#include <string>
#include <map>
#include <memory>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

import Indy.Graphics;
import Indy.VulkanGraphics;

namespace Indy::Graphics
{
	VulkanDriver::VulkanDriver()
	{
		if (!Initialize())
			throw std::runtime_error("Failed to initialize Vulkan!");
	}

	VulkanDriver::~VulkanDriver()
	{
		// Cleanup Core Resources
	}

	Driver::Type VulkanDriver::GetType()
	{
		return Driver::Type::Vulkan;
	}

	const RenderContext& VulkanDriver::CreateContext(const std::string& alias)
	{
		uint32_t id = m_Contexts.size();

		m_Contexts.emplace(id, std::make_unique<VulkanContext>(m_Instance, id));

		return *m_Contexts[id];
	}

	const RenderContext& VulkanDriver::AddContext(const RenderContext& context)
	{
		return *m_Contexts[0];
	}

	bool VulkanDriver::RemoveContext(const uint32_t& id)
	{
		return false;
	}

	const RenderContext& VulkanDriver::GetContext(const uint32_t& id)
	{
		return *m_Contexts[0];
	}

	const RenderContext& VulkanDriver::GetContext(const std::string& alias)
	{
		return *m_Contexts[0];
	}

	bool VulkanDriver::SetActiveContext(const uint32_t& id)
	{
		return false;
	}

	bool VulkanDriver::SetActiveContext(const RenderContext& context)
	{
		return false;
	}

	bool VulkanDriver::SetActiveViewport(const uint32_t& id)
	{
		return false;
	}

	bool VulkanDriver::SetActiveViewport(const std::string& alias)
	{
		return false;
	}

	bool VulkanDriver::Render(const Camera& camera)
	{
		return false;
	}

	bool VulkanDriver::Initialize()
	{
		// Vulkan Application Info
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 261);

		// Vulkan Instance Create Info
		VkInstanceCreateInfo instanceCreateInfo{};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &appInfo;

		std::vector<const char*> extensions = GetGLFWExtensions();

#ifdef ENGINE_DEBUG
		if (!QueryValidationLayerSupport({}))
			return false;

		// Attach validation layers
		instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(g_Vulkan_Validation_Layers.size());
		instanceCreateInfo.ppEnabledLayerNames = g_Vulkan_Validation_Layers.data();

		// Setup Debug Messenger
		VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
		debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugMessengerCreateInfo.pfnUserCallback = Vulkan_DebugCallback;
		debugMessengerCreateInfo.pUserData = nullptr;

		// Attach debug messenger to instance to track instance messages
		instanceCreateInfo.pNext = &debugMessengerCreateInfo;

		extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#else
		instanceCreateInfo.enabledLayerCount = 0;
		instanceCreateInfo.pNext = nullptr;
#endif

		// Attach required extensions
		instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

		// Create the Vulkan Instance
		if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance) != VK_SUCCESS)
		{
			INDY_CORE_CRITICAL("Failed to create Vulkan instance.");
			return false;
		}

#ifdef ENGINE_DEBUG
		ConfigureVulkanLogicalDevices();
		return CreateDebugMessenger(debugMessengerCreateInfo);
#endif

		ConfigureVulkanLogicalDevices();
		return true;
	}

	bool VulkanDriver::QueryValidationLayerSupport(const std::vector<const char*>& layers)
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

	std::vector<const char*> VulkanDriver::GetGLFWExtensions()
	{
		// Retrieve Required Extensions for GLFW
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		if (glfwExtensionCount == 0 || !glfwExtensions)
		{
			INDY_CORE_CRITICAL("GLFW is either unavailable or hasn't been initialized.");
			return std::vector<const char*>();
		}

		return std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
	}

	bool VulkanDriver::CreateDebugMessenger(const VkDebugUtilsMessengerCreateInfoEXT& info)
	{
		// For more general vulkan debugging, an explicit debug messenger must be created
		if (Vulkan_CreateDebugUtilsMessengerEXT(m_Instance, &info, nullptr, &m_DebugMessenger) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create debug messenger.");
			return false;
		}

		return true;
	}

	void VulkanDriver::ConfigureVulkanLogicalDevices()
	{
		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
			VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
		};

		// Core Vulkan Features
		VkPhysicalDeviceFeatures features{};

		// Vulkan 1.2 features
		VkPhysicalDeviceVulkan12Features features12{};
		features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		features12.bufferDeviceAddress = VK_TRUE;
		features12.descriptorIndexing = VK_TRUE;

		// Vulkan 1.3 features
		VkPhysicalDeviceVulkan13Features features13{};
		features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		features13.dynamicRendering = VK_TRUE;
		features13.synchronization2 = VK_TRUE;
		features13.pNext = &features12;

		// Package core and new features
		VkPhysicalDeviceFeatures2 deviceFeatures2{};
		deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		deviceFeatures2.features = features;
		deviceFeatures2.pNext = &features13;

		/*
		
		m_DeviceConfig.features = deviceFeatures2;
		m_DeviceConfig.extensionCount = static_cast<uint32_t>(deviceExtensions.size());
		m_DeviceConfig.extensions = deviceExtensions.data();
		m_DeviceConfig.layerCount = static_cast<uint32_t>(g_Vulkan_Validation_Layers.size());
		m_DeviceConfig.layers = g_Vulkan_Validation_Layers.data();

		*/
	}
}