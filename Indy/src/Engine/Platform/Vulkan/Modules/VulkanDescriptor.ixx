module;

#include <vector>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Descriptor;

export
{
	namespace Indy
	{
		class VulkanDescriptorSetBuilder
		{
		public:
			VulkanDescriptorSetBuilder() = default;
			~VulkanDescriptorSetBuilder() = default;

			void AddBinding(const VkDescriptorType& type, const uint32_t& binding, const uint32_t& count);
			void Clear();

			VkDescriptorSetLayout Build(
				const VkDevice& logicalDevice,
				const VkShaderStageFlags& stageFlags,
				void* pNext = nullptr,
				const VkDescriptorSetLayoutCreateFlags& flags = 0
			);

			std::vector<VkDescriptorSetLayoutBinding> GetBindings() { return m_Bindings; };

		private:
			std::vector<VkDescriptorSetLayoutBinding> m_Bindings;
		};

		struct VulkanDescriptorUpdateInfo
		{
			VkDescriptorSet set;
			VkDescriptorType type;
			uint32_t binding;
			uint32_t elementOffset;
			uint32_t count;
		};

		class VulkanDescriptorSetModifier
		{
		public:
			VulkanDescriptorSetModifier() = default;
			~VulkanDescriptorSetModifier() = default;

			void UpdateBufferBinding(const VulkanDescriptorUpdateInfo& updateInfo, VkDescriptorBufferInfo* bufferInfos);

			void UpdateImageBinding(const VulkanDescriptorUpdateInfo& updateInfo, VkDescriptorImageInfo* imageInfos);

			void ModifySets(const VkDevice& logicalDevice);

		private:
			std::vector<VkWriteDescriptorSet> m_Writes;
		};

		class VulkanDescriptorSetAllocator
		{
		public:
			VulkanDescriptorSetAllocator(const VkDescriptorPool& pool);
			~VulkanDescriptorSetAllocator() = default;

			VkDescriptorSet Allocate(const VkDevice& logicalDevice, const VkDescriptorSetLayout& layout);
			void ClearDescriptors(const VkDevice& logicalDevice);

		private:
			VkDescriptorPool m_Pool;
		};
	}
}