module;

#include <Engine/Core/LogMacros.h>
#include <cstdint>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Utils;

export
{
	namespace Indy::Graphics
	{
		typedef enum VulkanCompatibility : uint8_t {
			VULKAN_COMPATIBILITY_VOID = 0x00,
			VULKAN_COMPATIBILITY_PREFER,
			VULKAN_COMPATIBILITY_REQUIRED
		} VulkanCompatibility;

		// A container structure used for finding a GPU that meets some basic criteria.
		struct VulkanDeviceCompatibility
		{
			VulkanCompatibility graphics = VULKAN_COMPATIBILITY_VOID;
			VulkanCompatibility present = VULKAN_COMPATIBILITY_VOID;
			VulkanCompatibility compute = VULKAN_COMPATIBILITY_VOID;
			VulkanCompatibility geometryShader = VULKAN_COMPATIBILITY_VOID;
			VkPhysicalDeviceType type = VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM;
		};

		struct QueueFamilyIndices
		{
			std::optional<uint32_t> graphics;
			std::optional<uint32_t> present;
			std::optional<uint32_t> compute;

			bool Complete()
			{
				return graphics.has_value() && present.has_value() && compute.has_value();
			};
		};

		// A containing structure for Vulkan Physical Devices and their properties and features.
		struct VulkanPhysicalDevice
		{
			VkPhysicalDevice handle = VK_NULL_HANDLE;
			VkPhysicalDeviceProperties properties;
			VkPhysicalDeviceFeatures features;
			QueueFamilyIndices queueFamilies;
		};

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