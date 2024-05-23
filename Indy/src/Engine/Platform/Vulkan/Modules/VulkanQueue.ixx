module;

#include <optional>
#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Queue;

export
{
	namespace Indy::Graphics
	{
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