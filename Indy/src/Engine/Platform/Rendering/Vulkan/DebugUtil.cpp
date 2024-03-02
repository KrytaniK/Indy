#include "DebugUtil.h"

#include "Engine/Core/LogMacros.h"

namespace Engine
{
	namespace VulkanAPI
	{
		#ifdef ENGINE_DEBUG
			bool DebugUtil::s_Enabled = true;
		#else
			bool DebugUtil::s_Enabled = false;
		#endif

		VkDebugUtilsMessengerEXT DebugUtil::s_Messenger = nullptr;

		void DebugUtil::Init()
		{
			DebugUtil::QueryValidationLayerSupport();
		}

		void DebugUtil::Shutdown(VkInstance instance)
		{
			DebugUtil::DestroyMessenger(instance);
		}

		bool DebugUtil::QueryValidationLayerSupport()
		{
			if (!s_Enabled)
			{
				INDY_CORE_WARN("[Vulkan Debug] Validation Layers are disabled");
				return false;
			}

			// Request the number of layers
			uint32_t layerCount;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

			// Query for layer details
			std::vector<VkLayerProperties> availableLayers(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

			// Ensure all layers specified in c_ValidationLayers actually exist
			for (const char* layerName : c_ValidationLayers) {
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

		void DebugUtil::PopulateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
		{
			createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			createInfo.pfnUserCallback = Vulkan_DebugCallback;
			createInfo.pUserData = nullptr;
		}

		void DebugUtil::CreateMessenger(VkInstance instance)
		{
			if (!s_Enabled) return;

			if (s_Messenger)
			{
				INDY_CORE_WARN("[Vulkan Debug] Messenger already exists!");
				return;
			}

			VkDebugUtilsMessengerCreateInfoEXT createInfo{};
			DebugUtil::PopulateMessengerCreateInfo(createInfo);

			if (Vulkan_CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &s_Messenger) != VK_SUCCESS)
			{
				INDY_CORE_ERROR("[Vulkan Debug] Failed to set up Debug Messenger!");
			}
		}

		void DebugUtil::DestroyMessenger(VkInstance instance)
		{
			if (!s_Enabled) return;

			Vulkan_DestroyDebugUtilsMessengerEXT(instance, s_Messenger, nullptr);
		}
	}
}