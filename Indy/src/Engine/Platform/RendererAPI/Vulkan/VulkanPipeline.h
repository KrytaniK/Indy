#pragma once

#include "VulkanDevice.h"
#include "VulkanSwapChain.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace Engine
{
	class VulkanPipeline
	{
	private:
		static VkRenderPass s_RenderPass;
		static VkPipelineLayout s_PipelineLayout;
		static VkPipeline s_GraphicsPipeline;

	public:
		static const VkRenderPass& GetRenderPass() { return s_RenderPass; };
		static const VkPipelineLayout& GetLayout() { return s_PipelineLayout; };
		static const VkPipeline& GetPipeline() { return s_GraphicsPipeline; };

		static void Init();
		static void Shutdown();

	private:
		static VkShaderModule CreateShaderModule(VkDevice logicalDevice, const std::vector<char>& shaderCode);
		static void CreateRenderPass(VkDevice logicalDevice, VkFormat swapChainImageFormat);
		static void CreateGraphicsPipeline(VkDevice logicalDevice, VkExtent2D swapChainExtent);

	private:
		VulkanPipeline();
	};
}