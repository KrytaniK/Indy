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
		/* ----------------------------------------

					  TODO: IMPLEMENT

		---------------------------------------- */
	}

	Driver::Type VulkanDriver::GetType()
	{
		return Driver::Type::Vulkan;
	}

	Context& VulkanDriver::CreateContext(const std::string& alias)
	{
		uint32_t id = static_cast<uint32_t>(m_Contexts.size());

		m_Contexts.emplace(id, std::make_unique<VulkanContext>(m_Instance, id, alias, m_DeviceConfig));

		return *m_Contexts[id];
	}

	bool VulkanDriver::RemoveContext(const uint32_t& id)
	{
		auto it = m_Contexts.find(id);

		// Error if no context with that id was found
		if (it == m_Contexts.end())
		{
			INDY_CORE_ERROR("Could not find a Vulkan Render Context with ID: {0}", id);
			return false;
		}

		// Remove the entry, forcing destructor call of VulkanContext
		m_Contexts.erase(it);

		return true;
	}

	Context& VulkanDriver::GetContext(const uint32_t& id)
	{
		if (m_Contexts.size() == 0)
			throw std::runtime_error("Failed to get graphics context: No Vulkan Contexts Exist!");

		auto it = m_Contexts.find(id);

		// Error if no context with that id was found
		if (it == m_Contexts.end())
		{
			INDY_CORE_ERROR("Could not find a Vulkan Render Context with ID: {0}", id);
			return *m_Contexts[0];
		}

		return *it->second;
	}

	Context& VulkanDriver::GetContext(const std::string& alias)
	{
		uint32_t id = UINT32_MAX;

		for (auto& context : m_Contexts)
		{
			if (context.second->GetAlias() == alias)
			{
				// Check against duplicate aliases
				if (id != UINT32_MAX)
				{
					INDY_CORE_WARN("Duplicate Vulkan Context with alias: '{0}' found [ID of {1}]. Returning only the first instance!", alias, context.second->GetID());
					continue;
				}

				id = context.second->GetID();
			}
		}

		// Always return default context
		if (id == UINT32_MAX)
			return *m_Contexts.begin()->second;

		return *m_Contexts[id];
	}

	bool VulkanDriver::SetActiveContext(const uint32_t& id)
	{
		auto it = m_Contexts.find(id);

		if (it == m_Contexts.end())
		{
			INDY_CORE_ERROR("Failed to set the active Vulkan context. Invalid Context ID!");
			return false;
		}

		m_ActiveContext = id;

		return true;
	}

	bool VulkanDriver::SetActiveViewport(const uint32_t& id)
	{
		if (m_ActiveContext == UINT32_MAX)
		{
			INDY_CORE_ERROR("Cannot set the active viewport when no graphics context is active!");
			return false;
		}

		return m_Contexts[m_ActiveContext]->SetActiveViewport(id);
	}

	bool VulkanDriver::SetActiveViewport(const std::string& alias)
	{
		if (m_ActiveContext == UINT32_MAX)
		{
			INDY_CORE_ERROR("Cannot set the active viewport when no graphics context is active!");
			return false;
		}

		return m_Contexts[m_ActiveContext]->SetActiveViewport(alias);
	}

	bool VulkanDriver::Render(const Camera& camera)
	{
		/* ----------------------------------------
		
					  TODO: IMPLEMENT

		---------------------------------------- */

		return false;
	}

	bool VulkanDriver::Initialize()
	{
		// Vulkan Application Info
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 283);

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
		// Enumerate and store all physical devices
		uint32_t deviceCount = 0;
		if (vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to enumerate physical devices...");
			return;
		}

		// Ensure a valid device actually exists
		if (deviceCount == 0)
		{
			INDY_CORE_ERROR("Could not find any GPUs with Vulkan support!");
			return;
		}

		// Resize the container and pull all devices
		m_PhysicalDevices.resize(deviceCount);
		if (vkEnumeratePhysicalDevices(m_Instance, &deviceCount, m_PhysicalDevices.data()) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to enumerate physical devices...");
			return;
		}

		// Vulkan 1.2 features
		m_DeviceConfig.features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		m_DeviceConfig.features12.bufferDeviceAddress = VK_TRUE;
		m_DeviceConfig.features12.descriptorIndexing = VK_TRUE;

		// Vulkan 1.3 features
		m_DeviceConfig.features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		m_DeviceConfig.features13.dynamicRendering = VK_TRUE;
		m_DeviceConfig.features13.synchronization2 = VK_TRUE;
		m_DeviceConfig.features13.pNext = &m_DeviceConfig.features12;

		// Package core and new features
		m_DeviceConfig.features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		m_DeviceConfig.features.features = m_DeviceConfig.coreFeatures;
		m_DeviceConfig.features.pNext = &m_DeviceConfig.features13;

		// Attach physical device list
		m_DeviceConfig.physicalDeviceCount = deviceCount;
		m_DeviceConfig.physicalDevices = m_PhysicalDevices.data();

		// Attach required device extensions
		m_DeviceConfig.extensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
			VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
		};

#ifdef ENGINE_DEBUG
		// Attatch debug layers
		m_DeviceConfig.debugLayers = g_Vulkan_Validation_Layers;
#endif
	}
}