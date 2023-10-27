#pragma once

#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Engine
{
	struct VulkanCommandPoolInfo
	{
		VkCommandPool commandPool;
		VkCommandBuffer commandBuffer;
	};

	class VulkanCommandPool
	{
	private:
		static VulkanCommandPoolInfo s_CommandPoolInfo;

	public:
		static const VulkanCommandPoolInfo& GetCommandPoolInfo() { return s_CommandPoolInfo;  };

	public:
		void Init();
		void Shutdown();

		void RecordCommandBuffer(VkCommandBuffer buffer, const std::vector<VkFramebuffer>& framebuffers, uint32_t imageIndex);

	private:
		void CreateCommandBuffer();
	};
}