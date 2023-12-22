#pragma once

#include "Util.h"

#include <vulkan/vulkan.h>

namespace Engine::VulkanAPI
{
	class CommandPool
	{
	private:
		static VkCommandPool s_CommandPool;
		static std::vector<DrawCallInfo> s_DrawQueue;

	public:
		static const VkCommandPool& GetCommandPool() { return s_CommandPool; };

		static void Init(const VkDevice& logicalDevice, const Queue& graphicsQueue);
		static void Shutdown(const VkDevice& logicalDevice);

		static void CreateCommandBuffer(const VkDevice& logicalDevice, VkCommandBuffer& dstBuffer);

		static void RecordCommandBuffer_Begin(Viewport& viewport, VkRenderPass renderPass, VkPipeline graphicsPipeline);
		static void RecordCommandBuffer_End(Viewport& viewport, Camera& camera, VkPipelineLayout pipelineLayout);

		static void SubmitDrawCall(void* vertices, uint32_t vertexCount, void* indices, uint32_t indexCount, uint32_t instanceCount);
		
		static void FlushDrawQueue() { s_DrawQueue.clear(); };
	};
}