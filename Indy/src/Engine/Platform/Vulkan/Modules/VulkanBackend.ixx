module;

#include <vector>
#include <memory>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

export module Indy.VulkanRenderer:Backend;

import :Device;
import :Utils;

export
{
	namespace Indy
	{
		// Vulkan Globals
		// -------------

		const std::vector<const char*> g_Vulkan_Validation_Layers = {
				"VK_LAYER_KHRONOS_validation"
		};

		VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan_DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData
		);

		VkResult Vulkan_CreateDebugUtilsMessengerEXT(
			VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger
		);

		void Vulkan_DestroyDebugUtilsMessengerEXT(
			VkInstance instance,
			VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks* pAllocator
		);

		// Vulkan Backend Class
		// --------------------

		class VulkanBackend
		{
		// Static methods for retrieving shared resources.
		public:
			static VkInstance* GetInstance();
			static VulkanSharedResources* GetSharedResources();

		// Static Methods for Vulkan Initialization and cleanup
		private:
			static bool Init();
			static void Cleanup();
			static bool SupportsValidationLayers();

		private:
			inline static uint32_t s_ReferenceCount = 0;
			inline static VulkanSharedResources s_SharedResources;
			inline static VkDebugUtilsMessengerEXT s_DebugMessenger = VK_NULL_HANDLE;

		public:
			VulkanBackend();
			~VulkanBackend();
		};
	}
}