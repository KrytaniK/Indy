module;

#include <Engine/Core/LogMacros.h>
#include <cstdint>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Utils;

export
{
	namespace Indy
	{
		// -------------------------------
		// Debug Messenger Utility Methods
		// -------------------------------

		VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan_DebugCallback(
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
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				INDY_CORE_TRACE("[Vulkan Debug] {0}", pCallbackData->pMessage);
				break;
			default:
				return VK_FALSE;
				// INDY_CORE_INFO("[Vulkan Debug] {0}", pCallbackData->pMessage);
			}

			return VK_FALSE;
		}

		VkResult Vulkan_CreateDebugUtilsMessengerEXT(
			VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger
		) {
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

		void Vulkan_DestroyDebugUtilsMessengerEXT(
			VkInstance instance,
			VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks* pAllocator
		) {
			// Retrieve debug messenger destroy function from Vulkan
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

			// Execute function
			if (func != nullptr) {
				func(instance, debugMessenger, pAllocator);
			}
		}
	}
}