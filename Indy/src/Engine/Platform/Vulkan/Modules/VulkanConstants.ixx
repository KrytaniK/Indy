module;

#include <vector>

export module Indy.VulkanGraphics:Constants;

export
{
	namespace Indy::Graphics
	{
		const std::vector<const char*> g_Vulkan_Validation_Layers = {
				"VK_LAYER_KHRONOS_validation"
		};
	}
}