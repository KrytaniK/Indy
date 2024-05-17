module;

#include <optional>
#include <vulkan/vulkan.h>

export module Indy.VulkanRenderer:Queue;

import :Utils;

export
{
	namespace Indy
	{
		struct QueueFamilyIndices
		{
			std::optional<uint32_t> graphics;
			std::optional<uint32_t> present;
			std::optional<uint32_t> compute;

			bool Complete();
		};

		class VulkanQueue
		{
		public:
			static QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& device, const VulkanDeviceCompatibility& compatibility);

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