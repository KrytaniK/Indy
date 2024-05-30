module;

#include <vector>
#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:DescriptorSetLayout;

export
{
	namespace Indy
	{
		class VulkanDescriptorSetLayoutBuilder
		{
		public:
			VulkanDescriptorSetLayoutBuilder(const VkDevice& logicalDevice);
			~VulkanDescriptorSetLayoutBuilder() = default;

			void AddBinding(const uint32_t& binding, const VkDescriptorType& type);

			void Clear();

			VkDescriptorSetLayout Build(const VkShaderStageFlags& shaderStages, void* pNext = nullptr, const VkDescriptorSetLayoutCreateFlags& flags = 0);

		private:
			VkDevice m_LogicalDevice;
			std::vector<VkDescriptorSetLayoutBinding> m_Bindings;
		};
	}
}