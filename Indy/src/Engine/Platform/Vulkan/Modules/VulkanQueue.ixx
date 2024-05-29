module;

#include <optional>
#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Queue;

export
{
	namespace Indy
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
			~VulkanQueue() = default;

			const VkQueue& GetComputeQueue() const { return m_Compute; };
			const VkQueue& GetGraphicsQueue() const { return m_Graphics; };
			const VkQueue& GetPresentQueue() const { return m_Present; };

		private:
			VkDevice m_LogicalDevice = VK_NULL_HANDLE;
			VkQueue m_Compute = VK_NULL_HANDLE;
			VkQueue m_Present = VK_NULL_HANDLE;
			VkQueue m_Graphics = VK_NULL_HANDLE;
		};
	}
}