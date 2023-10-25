#include "VulkanDebugUtil.h"

#include "Engine/Core/Log.h"

namespace Engine
{
	// -------------------------------------
	// Global Vulkan Debug Utility Functions (Vulkan doesn't load these automatically)
	// -------------------------------------

	static VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan_DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	) {

		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			INDY_CORE_ERROR("[Vulkan Debug] {0}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			INDY_CORE_WARN("[Vulkan Debug] {0}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			INDY_CORE_INFO("[Vulkan Debug] {0}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			INDY_CORE_TRACE("[Vulkan Debug] {0}", pCallbackData->pMessage);
			break;
		default:
			INDY_CORE_INFO("[Vulkan Debug] {0}", pCallbackData->pMessage);
		}

		return VK_FALSE;
	}

	static VkResult Vulkan_CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	static void Vulkan_DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}

	// --------------------
	// Class implementation
	// --------------------

	bool VulkanDebugUtil::QueryValidationLayerSupport()
	{
		if (!m_Enabled) return false;

		// Request the number of layers
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		// Query for layer details
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		// Ensure all layers specified in m_ValidationLayers actually exist
		for (const char* layerName : m_ValidationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				INDY_CORE_ERROR("[Vulkan Debug] Validation layer requested, but is missing!: '{0}'", layerName);
				return false;
			}
		}

		return true;
	}

	void VulkanDebugUtil::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = Vulkan_DebugCallback;
		createInfo.pUserData = nullptr;
	}

	void VulkanDebugUtil::CreateDebugMessenger(VkInstance instance)
	{
		if (!m_Enabled) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		PopulateDebugMessengerCreateInfo(createInfo);

		if (Vulkan_CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &m_Messenger) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("[Vulkan Debug] Failed to set up Debug Messenger!");
		}
	}

	void VulkanDebugUtil::DestroyDebugMessenger(VkInstance instance)
	{
		if (!m_Enabled) return;

		Vulkan_DestroyDebugUtilsMessengerEXT(instance, m_Messenger, nullptr);
	}
}