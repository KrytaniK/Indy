module;

#include <vector>
#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Constants;

export
{
	namespace Indy
	{
		const std::vector<const char*> g_Vulkan_Validation_Layers = {
			"VK_LAYER_KHRONOS_validation"
		};

		const std::vector<const char*> g_Vulkan_Device_Extensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
			VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
		};

		// Allows for the buffering of frames (triple-buffering in this case)
		constexpr uint8_t g_Max_Frames_In_Flight = 3;
	}
}