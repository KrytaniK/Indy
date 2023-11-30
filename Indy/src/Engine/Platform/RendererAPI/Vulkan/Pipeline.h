#pragma once

#include "Util.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace Engine::VulkanAPI
{
	class Pipeline
	{
	private:
		static VkRenderPass s_RenderPass;
		static VkPipelineLayout s_PipelineLayout;
		static VkPipeline s_GraphicsPipeline;

	public:
		static const VkRenderPass& GetRenderPass() { return s_RenderPass; };
		static const VkPipelineLayout& GetLayout() { return s_PipelineLayout; };
		static const VkPipeline& GetPipeline() { return s_GraphicsPipeline; };

		static void Init(const VkDevice& logicalDevice, const Viewport& viewport);
		static void Shutdown(const VkDevice& logicalDevice);

	private:
		static VkShaderModule CreateShaderModule(const VkDevice& logicalDevice, const std::vector<char>& shaderCode);
		static void CreateRenderPass(const VkDevice& logicalDevice, VkFormat swapChainImageFormat);
		static void CreateGraphicsPipeline(const VkDevice& logicalDevice, VkExtent2D swapChainExtent);

	private:
		Pipeline();
	};
}