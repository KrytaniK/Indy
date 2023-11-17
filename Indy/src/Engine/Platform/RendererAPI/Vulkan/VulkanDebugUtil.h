#pragma once

#include <vulkan\vulkan.h>

#include <vector>

namespace Engine
{

	class VulkanDebugUtil
	{
	private:
		static bool s_Enabled;
		static VkDebugUtilsMessengerEXT s_Messenger;
		static std::vector<const char*> s_ValidationLayers;

	public:
		static void Init();
		static void Shutdown(VkInstance instance);
		static bool Enabled() { return s_Enabled; };

		static uint32_t GetValidationLayerCount() { return static_cast<uint32_t>(s_ValidationLayers.size()); };
		static const char* const* GetValidationLayerNames() { return s_ValidationLayers.data(); };

		static void PopulateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		static void CreateMessenger(VkInstance instance);

	private:
		static bool QueryValidationLayerSupport();
		static void DestroyMessenger(VkInstance instance);

	private:
		VulkanDebugUtil();
	};
}