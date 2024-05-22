module;

#include <optional>
#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Queue;

import :Utils;

export
{
	namespace Indy::Graphics
	{
		class VulkanQueue
		{
		public:
			VulkanQueue(const VkDevice& logicalDevice, const QueueFamilyIndices& queueIndices);

		private:
			VkDevice m_LogicalDevice = VK_NULL_HANDLE;
			VkQueue m_Graphics = VK_NULL_HANDLE;
			VkQueue m_Present = VK_NULL_HANDLE;
			VkQueue m_Compute = VK_NULL_HANDLE;
		};
	}
}