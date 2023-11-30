#pragma once

#include "Util.h"

#include <vulkan/vulkan.h>

namespace Engine::VulkanAPI
{
	class CommandPool
	{
	private:
		static VkCommandPool s_CommandPool;

	public:
		static const VkCommandPool& GetCommandPool() { return s_CommandPool; };

		static void Init(const VkDevice& logicalDevice, const Queue& graphicsQueue);
		static void Shutdown(const VkDevice& logicalDevice);

		static void CreateCommandBuffer(const VkDevice& logicalDevice, VkCommandBuffer& dstBuffer);

		static void RecordCommandBuffer_Begin(Viewport& viewport, VkRenderPass renderPass, VkPipeline graphicsPipeline);
		static void RecordCommandBuffer_End(Viewport& viewport, VkPipelineLayout pipelineLayout);

	private:
		
	};
}