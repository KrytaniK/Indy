#pragma once

#include "VulkanDevice.h"
#include "VulkanSwapChain.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace Engine
{
	struct VulkanPipelineInfo
	{
		VkRenderPass renderPass;
		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;
	};

	class VulkanPipeline
	{
	private:
		static VulkanPipelineInfo s_PipelineInfo;

	public:
		static const VulkanPipelineInfo& GetPipelineInfo() { return s_PipelineInfo; };

	public:
		void Init();
		void Shutdown();

	private:
		VkShaderModule CreateShaderModule(VkDevice logicalDevice, const std::vector<char>& shaderCode);
		void CreateRenderPass(VkDevice logicalDevice, VkFormat swapChainImageFormat);
		void CreateGraphicsPipeline(VkDevice logicalDevice, VkExtent2D swapChainExtent);
	};
}