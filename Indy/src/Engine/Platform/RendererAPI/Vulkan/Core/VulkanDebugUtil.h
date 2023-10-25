#pragma once

#include <vulkan\vulkan.h>

#include <vector>

namespace Engine
{

	class VulkanDebugUtil
	{
	public:
		bool Enabled() { return m_Enabled; };

		// Validation Layers
		bool QueryValidationLayerSupport();

		uint32_t GetValidationLayerCount() { return static_cast<uint32_t>(m_ValidationLayers.size()); };
		const char* const* GetValidationLayerNames() { return m_ValidationLayers.data(); };

		// Debug Messenger
		VkDebugUtilsMessengerEXT GetMessenger() { return m_Messenger; };
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		void CreateDebugMessenger(VkInstance instance);
		void DestroyDebugMessenger(VkInstance instance);

	private:
		const std::vector<const char*> m_ValidationLayers = {
			"VK_LAYER_KHRONOS_validation",
		};

		VkDebugUtilsMessengerEXT m_Messenger;

		#ifdef ENGINE_DEBUG
			const bool m_Enabled = true;
		#else
			const bool m_Enabled = false;
		#endif
	};
}