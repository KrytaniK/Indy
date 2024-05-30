module;

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Descriptor;

export
{
	namespace Indy
	{
		class VulkanDescriptor
		{
		public:
			VulkanDescriptor(const VkDescriptorSet& set, const VkDescriptorSetLayout& layout)
				: m_Set(set), m_Layout(layout) {};

			~VulkanDescriptor() = default;

			const VkDescriptorSet& GetSet() const { return m_Set; };
			const VkDescriptorSetLayout& GetSetLayout() const { return m_Layout; };

		private:
			VkDescriptorSet m_Set;
			VkDescriptorSetLayout m_Layout;
		};
	}
}