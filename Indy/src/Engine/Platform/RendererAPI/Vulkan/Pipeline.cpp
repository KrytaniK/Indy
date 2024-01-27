#include "Pipeline.h"

#include "Engine/Core/LogMacros.h"
#include "DescriptorPool.h"

#include <fstream>

namespace Engine::VulkanAPI
{
	// ----------------------------------
	// Util functions for loading shaders (Temp)
	// ----------------------------------

	static const std::vector<char> readFile(const std::string& filePath)
	{
		std::ifstream file(filePath, std::ios::ate | std::ios::binary);

		if (!file || !file.is_open())
		{
			INDY_CORE_ERROR("[Pipeline] Failed to open file: '{0}'", filePath);
		}

		// Get File Size
		size_t fileSize = (size_t)file.tellg();

		// Create a buffer storing file data
		std::vector<char> buffer(fileSize);

		// Read file, copying to buffer
		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

	// -----------------
	// Class Definitions
	// -----------------

	VkRenderPass Pipeline::s_RenderPass;
	VkPipelineLayout Pipeline::s_PipelineLayout;
	VkPipeline Pipeline::s_GraphicsPipeline;

	void Pipeline::Init(const VkDevice& logicalDevice, const Viewport& viewport)
	{
		CreateRenderPass(logicalDevice, viewport.imageFormat);
		CreateGraphicsPipeline(logicalDevice, viewport.extent);
	}

	void Pipeline::Shutdown(const VkDevice& logicalDevice)
	{
		vkDestroyPipeline(logicalDevice, s_GraphicsPipeline, nullptr);
		vkDestroyPipelineLayout(logicalDevice, s_PipelineLayout, nullptr);
		vkDestroyRenderPass(logicalDevice, s_RenderPass, nullptr);
	}

	VkShaderModule Pipeline::CreateShaderModule(const VkDevice& logicalDevice, const std::vector<char>& spvCode)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = spvCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(spvCode.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("[Pipeline] Failed to create shader module!");
		}

		return shaderModule;
	}

	void Pipeline::CreateRenderPass(const VkDevice& logicalDevice, VkFormat swapChainImageFormat)
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // Only one sample without multisampling

		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Present to Swap Chain

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0; // Referenced by fragment shader "layout(location = 0)"
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;

		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;

		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;


		if (vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr, &s_RenderPass) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("[Pipeline] Failed to create Render Pass!");
		}
	}

	void Pipeline::CreateGraphicsPipeline(const VkDevice& logicalDevice, VkExtent2D swapChainExtent)
	{
		const std::string vertFilePathAbs = "C:/Dev/C++/Indy/Indy/src/Engine/Platform/RendererAPI/Vulkan/TestShaders/hellotrianglevert.spv";
		const std::string fragFilePathAbs = "C:/Dev/C++/Indy/Indy/src/Engine/Platform/RendererAPI/Vulkan/TestShaders/hellotrianglefrag.spv";

		auto testVertexShader = readFile(vertFilePathAbs);
		auto testFragmentShader = readFile(fragFilePathAbs);

		VkShaderModule vertShaderModule = CreateShaderModule(logicalDevice, testVertexShader);
		VkShaderModule fragShaderModule = CreateShaderModule(logicalDevice, testFragmentShader);

		// Vertex Shader Pipeline Stage
		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

		vertShaderStageInfo.module = vertShaderModule; // shader module
		vertShaderStageInfo.pName = "main"; // shader entry point

		// Fragment Shader Pipeline Stage
		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		// Shader Stages array
		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		// Vertex Input
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		// Vertex Attributes and Binding descriptions
		auto bindingDesc = Vertex::GetBindingDescription();
		auto attrDescs = Vertex::getAttributeDescriptions();

		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attrDescs.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDesc;
		vertexInputInfo.pVertexAttributeDescriptions = attrDescs.data();

		// Input Assembly
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		// Viewport State
		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		// Rasterizer
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE; // enabling this essentially disables framebuffer output.
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		// Multisampling
		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		// Color Blending
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		// Dynamic State
		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();


		std::vector<VkDescriptorSetLayout> descriptorLayouts = {
			DescriptorPool::GetDescriptorSetLayout(UBO_DESCRIPTOR_SET_LAYOUT)
		};

		// Pipeline Layout Creation
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1; // Optional
		pipelineLayoutInfo.pSetLayouts = descriptorLayouts.data(); // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &s_PipelineLayout) != VK_SUCCESS) {
			INDY_CORE_ERROR("failed to create pipeline layout!");
		}

		// Finally, create the graphics pipeline
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;

		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = nullptr; // Optional
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;

		pipelineInfo.layout = s_PipelineLayout;

		pipelineInfo.renderPass = s_RenderPass;
		pipelineInfo.subpass = 0;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &s_GraphicsPipeline) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("[Pipeline] Failed to create Graphics Pipeline!");
		}

		// Shader Module Cleanup
		vkDestroyShaderModule(logicalDevice, fragShaderModule, nullptr);
		vkDestroyShaderModule(logicalDevice, vertShaderModule, nullptr);
	}
}