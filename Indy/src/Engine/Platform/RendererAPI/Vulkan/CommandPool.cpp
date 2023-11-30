#include "CommandPool.h"

#include "Engine/Core/Log.h"

#include "Pipeline.h"
#include "SwapChain.h"

/*
*	Rendering begins with starting the recording phase and starting the render pass. The application will handle the gathering of all vertex and matrix data, which will be
*		submitted through the Renderer's exposed API. This means that calls to Renderer::Draw(), Renderer::DrawIndexed() and Renderer::DrawInstanced(), will push 'commands'
*		into a queue that the underlying API (Vulkan in this case) will interpret and transform into usable data, through a SubmitMeshData() function. All of this data will
*		be converted into special buffer containers that will be used during the END phase of command buffer recording.
*
*	Now, mesh locations will default all to the same origin (specific with the MVP matrices in the Uniform Buffer). The best way I see to achieve mesh rendering at
*		locations specific to each mesh, is to have two seperate uniform buffers. One for the camera (Projection matrix), and one for the Meshes (Model & View matrices), then use
*		Vulkan's dynamicOffsets to bind the descriptor set (Uniform Buffer Object in this case) for each mesh at its offset, given by the index of the mesh we're rendering multiplied by the alignment of the
*		Uniform Buffer Object.
*/

namespace Engine::VulkanAPI
{
	VkCommandPool CommandPool::s_CommandPool;

	void CommandPool::Init(const VkDevice& logicalDevice, const Queue& graphicsQueue)
	{
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = graphicsQueue.familyIndex;

		if (vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &s_CommandPool) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("[CommandPool] Failed to create command pool!");
		}
	}

	void CommandPool::Shutdown(const VkDevice& logicalDevice)
	{
		vkDestroyCommandPool(logicalDevice, s_CommandPool, nullptr);
	}

	void CommandPool::CreateCommandBuffer(const VkDevice& logicalDevice, VkCommandBuffer& dstBuffer)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = s_CommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, &dstBuffer) != VK_SUCCESS) {
			throw std::runtime_error("[CommandPool] Failed to allocate command buffer!");
		}
	}

	void CommandPool::RecordCommandBuffer_Begin(Viewport& viewport, VkRenderPass renderPass, VkPipeline graphicsPipeline)
	{
		Frame currentFrame = viewport.GetCurrentFrame();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		// Start the command buffer
		if (vkBeginCommandBuffer(currentFrame.commandBuffer, &beginInfo) != VK_SUCCESS) {
			INDY_CORE_ERROR("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = viewport.framebuffers[viewport.imageIndex];

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = viewport.extent;

		// Define Clear Color as black with 100% opacity
		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		// Start the render pass
		vkCmdBeginRenderPass(currentFrame.commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// Bind the graphics pipeline
		vkCmdBindPipeline(currentFrame.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	}

	void CommandPool::RecordCommandBuffer_End(Viewport& viewport, VkPipelineLayout pipelineLayout)
	{
		Frame currentFrame = viewport.GetCurrentFrame();

		// Viewport and Scissor state are dynamic, so they need to be specified in the command buffer before issuing the draw command.
		VkViewport vulkanViewport{};
		vulkanViewport.x = 0.0f;
		vulkanViewport.y = 0.0f;
		vulkanViewport.width = static_cast<float>(viewport.extent.width);
		vulkanViewport.height = static_cast<float>(viewport.extent.height);
		vulkanViewport.minDepth = 0.0f;
		vulkanViewport.maxDepth = 1.0f;
		vkCmdSetViewport(currentFrame.commandBuffer, 0, 1, &vulkanViewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = viewport.extent;
		vkCmdSetScissor(currentFrame.commandBuffer, 0, 1, &scissor);

		#pragma region Rendering Pseudocode

		#pragma endregion

		// End Render Pass
		vkCmdEndRenderPass(currentFrame.commandBuffer);

		// Command buffer cleanup
		if (vkEndCommandBuffer(currentFrame.commandBuffer) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("[VulkanCommandBuffer] Failed to record command buffer!");
		}
	}
}