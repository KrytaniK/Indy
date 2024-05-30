module;

#include <span>
#include <vector>
#include <string>
#include <unordered_map>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:DescriptorPool;

export
{
	namespace Indy
	{
		struct VulkanDescriptorLayoutBuilder
		{
			struct BuildInfo
			{
				VkShaderStageFlags shaderStages;
				void* pNext = nullptr;
				VkDescriptorSetLayoutCreateFlags flags = 0;
			};

			std::vector<VkDescriptorSetLayoutBinding> bindings;

			void AddBinding(const VkDescriptorType& type, const uint32_t& binding, const uint32_t& count);
			void Clear();

			VkDescriptorSetLayout Build(const VkDevice& logicalDevice, const BuildInfo& buildInfo);
		};

		class VulkanDescriptorPool
		{
		public:
			struct Ratio
			{
				VkDescriptorType type;
				float ratio;
			};

		public:
			VulkanDescriptorPool(const VkDevice& logicalDevice, const uint32_t& maxSets, std::span<Ratio>);
			~VulkanDescriptorPool();

			VkDescriptorSet AllocateDescriptorSet(const VkDescriptorSetLayout& layout);

			void ClearDescriptors();

		private:
			VkDevice m_LogicalDevice;
			VkDescriptorPool m_DescriptorPool;
		};
	}
}