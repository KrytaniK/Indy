#include "VulkanCommandPool.h"

#include "Engine/Core/Log.h"

#include "VulkanDevice.h"
#include "VulkanPipeline.h"
#include "VulkanSwapChain.h"

namespace Engine
{
	VulkanCommandPoolInfo VulkanCommandPool::s_CommandPoolInfo;

	void VulkanCommandPool::Init()
	{
		const VulkanDeviceInfo& deviceInfo = VulkanDevice::GetDeviceInfo();

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = deviceInfo.graphicsQueueFamilyIndex;

		if (vkCreateCommandPool(deviceInfo.logicalDevice, &poolInfo, nullptr, &s_CommandPoolInfo.commandPool) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("[VulkanCommandPool] Failed to create command pool!");
		}

		CreateCommandBuffer();
	}

	void VulkanCommandPool::Shutdown()
	{
		const VulkanDeviceInfo& deviceInfo = VulkanDevice::GetDeviceInfo();

		vkDestroyCommandPool(deviceInfo.logicalDevice, s_CommandPoolInfo.commandPool, nullptr);
	}
	void VulkanCommandPool::CreateCommandBuffer()
	{
		const VulkanDeviceInfo& deviceInfo = VulkanDevice::GetDeviceInfo();

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = s_CommandPoolInfo.commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(deviceInfo.logicalDevice, &allocInfo, &s_CommandPoolInfo.commandBuffer) != VK_SUCCESS) {
			INDY_CORE_ERROR("[VulkanCommandPool] Failed to allocate command buffers!");
		}
	}

	void VulkanCommandPool::RecordCommandBuffer(VkCommandBuffer buffer, const std::vector<VkFramebuffer>& framebuffers, uint32_t imageIndex)
	{
		const VulkanPipelineInfo& pipelineInfo = VulkanPipeline::GetPipelineInfo();
		const VulkanSwapchainInfo& swapchainInfo = VulkanSwapChain::GetSwapChainInfo();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		// Start the command buffer
		if (vkBeginCommandBuffer(s_CommandPoolInfo.commandBuffer, &beginInfo) != VK_SUCCESS) {
			INDY_CORE_ERROR("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = pipelineInfo.renderPass;
		renderPassInfo.framebuffer = framebuffers[imageIndex];

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchainInfo.extent;

		// Define Clear Color as black with 100% opacity
		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		// Start the render pass
		vkCmdBeginRenderPass(s_CommandPoolInfo.commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// Bind the graphics pipeline
		vkCmdBindPipeline(s_CommandPoolInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineInfo.graphicsPipeline);

		// Viewport and Scissor state are dynamic, so they need to be specified in the command buffer before issuing the draw command.
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapchainInfo.extent.width);
		viewport.height = static_cast<float>(swapchainInfo.extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(s_CommandPoolInfo.commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapchainInfo.extent;
		vkCmdSetScissor(s_CommandPoolInfo.commandBuffer, 0, 1, &scissor);

		// Issue the draw command (vertex information will later come from a vertex buffer)
		vkCmdDraw(s_CommandPoolInfo.commandBuffer, 3, 1, 0, 0);

		// End Render Pass
		vkCmdEndRenderPass(s_CommandPoolInfo.commandBuffer);

		// Command buffer cleanup
		if (vkEndCommandBuffer(s_CommandPoolInfo.commandBuffer) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("[VulkanCommandBuffer] Failed to record command buffer!");
		}
	}
}