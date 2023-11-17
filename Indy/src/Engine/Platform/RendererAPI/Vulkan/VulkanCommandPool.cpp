#include "VulkanCommandPool.h"

#include "Engine/Core/Log.h"

#include "VulkanDevice.h"
#include "VulkanPipeline.h"
#include "VulkanSwapChain.h"

namespace Engine
{
	VkCommandPool VulkanCommandPool::s_CommandPool;
	std::vector<VkCommandBuffer> VulkanCommandPool::s_CommandBuffers;

	void VulkanCommandPool::Init(uint32_t maxFramesInFlight)
	{
		const VkDevice& logicalDevice = VulkanDevice::GetLogicalDevice();
		const VulkanQueue& graphicsQueue = VulkanDevice::GetGraphicsQueue();

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = graphicsQueue.familyIndex;

		if (vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &s_CommandPool) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("[VulkanCommandPool] Failed to create command pool!");
		}

		VulkanCommandPool::CreateCommandBuffers(maxFramesInFlight);
	}

	void VulkanCommandPool::Shutdown()
	{
		const VkDevice& logicalDevice = VulkanDevice::GetLogicalDevice();

		vkDestroyCommandPool(logicalDevice, s_CommandPool, nullptr);
	}

	void VulkanCommandPool::CreateCommandBuffers(uint32_t maxFramesInFlight)
	{
		const VkDevice& logicalDevice = VulkanDevice::GetLogicalDevice();

		s_CommandBuffers.resize(maxFramesInFlight);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = s_CommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = maxFramesInFlight;

		if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, s_CommandBuffers.data()) != VK_SUCCESS) {
			INDY_CORE_ERROR("[VulkanCommandPool] Failed to allocate command buffers!");
		}
	}

	void VulkanCommandPool::RecordCommandBuffer(VkCommandBuffer buffer, const std::vector<VkFramebuffer>& framebuffers, uint32_t imageIndex)
	{
		const VkRenderPass& renderPass = VulkanPipeline::GetRenderPass();
		const VkPipeline& graphicsPipeline = VulkanPipeline::GetPipeline();

		const VkExtent2D& swapchainExtent = VulkanSwapChain::GetExtent();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		// Start the command buffer
		if (vkBeginCommandBuffer(buffer, &beginInfo) != VK_SUCCESS) {
			INDY_CORE_ERROR("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = framebuffers[imageIndex];

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchainExtent;

		// Define Clear Color as black with 100% opacity
		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		// Start the render pass
		vkCmdBeginRenderPass(buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// Bind the graphics pipeline
		vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

		// Viewport and Scissor state are dynamic, so they need to be specified in the command buffer before issuing the draw command.
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapchainExtent.width);
		viewport.height = static_cast<float>(swapchainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(buffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapchainExtent;
		vkCmdSetScissor(buffer, 0, 1, &scissor);

		// Issue the draw command (vertex information will later come from a vertex buffer)
		vkCmdDraw(buffer, 3, 1, 0, 0);

		// End Render Pass
		vkCmdEndRenderPass(buffer);

		// Command buffer cleanup
		if (vkEndCommandBuffer(buffer) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("[VulkanCommandBuffer] Failed to record command buffer!");
		}
	}
}