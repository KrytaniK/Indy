module;

#include <optional>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Queue;

export
{
	namespace Indy::Graphics
	{
		class VulkanQueue
		{
		public:
			VulkanQueue(const VkPhysicalDevice& physicalDevice);

			~VulkanQueue();

			const VkQueue& GetCompute();
			const std::optional<uint32_t>& GetComputeIndex();

			const VkQueue& GetGraphics();
			const std::optional<uint32_t>& GetGraphicsIndex();

			void Bind(const VkDevice& logicalDevice);

			bool HasDedicatedCompute();
			bool ComputeIsGraphics();

		private:
			VkQueue m_Compute;
			std::optional<uint32_t> m_ComputeIndex;

			VkQueue m_Graphics;
			std::optional<uint32_t> m_GraphicsIndex;
		};
	}
}