module;

#include <string>
#include <unordered_map>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:SyncObjects;

export
{
	namespace Indy
	{
		class VulkanSyncObjects
		{
		public:
			VulkanSyncObjects(const VkDevice& logicalDevice);
			~VulkanSyncObjects();

			void AddFence(const std::string& name, const VkFenceCreateFlags& flags);
			void AddSemaphore(const std::string& name, const VkSemaphoreCreateFlags& flags = 0);

			const VkFence& GetFence(const std::string& fence) const;
			const VkSemaphore& GetSemaphore(const std::string& semaphore) const;

		private:
			VkDevice m_LogicalDevice;
			std::unordered_map<std::string, VkFence> m_Fences;
			std::unordered_map<std::string, VkSemaphore> m_Semaphores;
		};
	}
}