#pragma once

#include "Util.h"

#include <vector>
#include <vulkan\vulkan.h>


namespace Engine
{
	namespace VulkanAPI
	{
		class DebugUtil
		{
		private:
			static bool s_Enabled;
			static VkDebugUtilsMessengerEXT s_Messenger;

		public:
			static void Init();
			static void Shutdown(VkInstance instance);
			static bool Enabled() { return s_Enabled; };

			static uint32_t GetValidationLayerCount() { return static_cast<uint32_t>(c_ValidationLayers.size()); };
			static const char* const* GetValidationLayerNames() { return c_ValidationLayers.data(); };

			static void PopulateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

			static void CreateMessenger(VkInstance instance);

		private:
			static bool QueryValidationLayerSupport();
			static void DestroyMessenger(VkInstance instance);

		private:
			DebugUtil();
		};
	}
}