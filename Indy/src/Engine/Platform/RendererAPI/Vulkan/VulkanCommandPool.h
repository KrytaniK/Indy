#pragma once

#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Engine
{
	class VulkanCommandPool
	{
	private:
		static VkCommandPool s_CommandPool;
		static std::vector<VkCommandBuffer> s_CommandBuffers;

	public:
		static const VkCommandPool& GetCommandPool() { return s_CommandPool; };
		static std::vector<VkCommandBuffer>& GetCommandBuffers() { return s_CommandBuffers; };

		static void Init(uint32_t maxFramesInFlight);
		static void Shutdown();
		
		static void RecordCommandBuffer(VkCommandBuffer buffer, const std::vector<VkFramebuffer>& framebuffers, uint32_t imageIndex);
	
	private:
		static void CreateCommandBuffers(uint32_t maxFramesInFlight);
	};
}