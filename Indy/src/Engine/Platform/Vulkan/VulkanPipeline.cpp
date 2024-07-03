#include <Engine/Core/LogMacros.h>

#include <vector>
#include <memory>
#include <cstdint>
#include <set>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <vulkan/vulkan.h>

import Indy.Graphics;
import Indy.VulkanGraphics;

namespace Indy
{
	// Pipeline Shader Helper Functions

	shaderc_shader_kind GetShadercType(const ShaderType& type)
	{
		switch (type)
		{
			case INDY_SHADER_TYPE_VERTEX: return shaderc_glsl_vertex_shader;
			case INDY_SHADER_TYPE_FRAGMENT: return shaderc_glsl_fragment_shader;
			case INDY_SHADER_TYPE_COMPUTE: return shaderc_glsl_compute_shader;
			case INDY_SHADER_TYPE_GEOMETRY: return shaderc_glsl_geometry_shader;
			default: return shaderc_vertex_shader;
		}
	}

	std::vector<uint32_t> CompileSPIRVFromGLSL(const VulkanShader& shader)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		options.SetOptimizationLevel(shaderc_optimization_level_performance);

		options.SetSourceLanguage(shaderc_source_language_glsl);
		shaderc::SpvCompilationResult spvModule = compiler.CompileGlslToSpv(
			shader.fileContent, GetShadercType(shader.type), shader.name.c_str(), options
		);

		if (spvModule.GetCompilationStatus() != shaderc_compilation_status_success) {
			INDY_CORE_ERROR("GLSL COMPILATION ERROR: {0}", spvModule.GetErrorMessage());
			return {};
		}

		return std::vector<uint32_t>(spvModule.begin(), spvModule.end());
	}

	void ReflectVulkanShader(const VulkanShader& shader, VulkanDescriptorSetBuilder& layoutBuilder, std::vector<VkDescriptorPoolSize>& poolSizes, std::vector<VkPushConstantRange>& pcRanges)
	{
		// Set up spirv_cross compiler
		spirv_cross::Compiler compiler(shader.spv);

		// Reflect compute shader and retrieve resources
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		// Uniform Buffers (such as: uniform UBO)
		VkDescriptorPoolSize uboPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0 }; // or Uniform_Buffer_Dynamic
		for (const spirv_cross::Resource& res : resources.uniform_buffers)
		{
			uboPoolSize.descriptorCount++;

			unsigned binding = compiler.get_decoration(res.id, spv::DecorationBinding);
			layoutBuilder.AddBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, binding, 1);
		}

		if (uboPoolSize.descriptorCount)
			poolSizes.emplace_back(uboPoolSize);

		// Storage Buffers (such as: buffer SSBO)
		VkDescriptorPoolSize sboPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0 }; // or Storage_Buffer_Dynamic
		for (const spirv_cross::Resource& res : resources.storage_buffers)
		{
			sboPoolSize.descriptorCount++;

			unsigned binding = compiler.get_decoration(res.id, spv::DecorationBinding);
			layoutBuilder.AddBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, binding, 1);
		}

		if (sboPoolSize.descriptorCount)
			poolSizes.emplace_back(sboPoolSize);

		// Stage Inputs (such as: [in vec2 uv] in vertex shader)
		// VkVertexInputAttributeDescription
		//for (const spirv_cross::Resource& res : resources.stage_inputs)
		//{
		//	unsigned set = compiler.get_decoration(res.id, spv::DecorationDescriptorSet);
		//	unsigned binding = compiler.get_decoration(res.id, spv::DecorationBinding);
		//}

		// Stage Outputs (such as: [out vec4 FragColor] in fragment shader)
		// Maps to several things, but particularly useful to set write mask to 0 in 
		//	VkPipelineColorBlendAttachmentState if a location is not written to in shader.
		//for (const spirv_cross::Resource& res : resources.stage_outputs)
		//{
		//	unsigned set = compiler.get_decoration(res.id, spv::DecorationDescriptorSet);
		//	unsigned binding = compiler.get_decoration(res.id, spv::DecorationBinding);
		//}

		// Subpass Inputs (such as: subpassInput)
		// VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
		//for (const spirv_cross::Resource& res : resources.subpass_inputs)
		//{
		//	unsigned set = compiler.get_decoration(res.id, spv::DecorationDescriptorSet);
		//	unsigned binding = compiler.get_decoration(res.id, spv::DecorationBinding);
		//}

		// Storage Images (such as: image2D)
		VkDescriptorPoolSize storageImagePoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0 };
		VkDescriptorPoolSize storageTexelBufferPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 0 };
		for (const spirv_cross::Resource& res : resources.storage_images)
		{
			const auto& type = compiler.get_type(res.type_id);
			unsigned binding = compiler.get_decoration(res.id, spv::DecorationBinding);

			if (type.image.dim == spv::DimBuffer)
			{
				storageTexelBufferPoolSize.descriptorCount++;
				layoutBuilder.AddBinding(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, binding, 1);
			}
			else
			{
				storageImagePoolSize.descriptorCount++;
				layoutBuilder.AddBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, binding, 1);
			}
		}

		if (storageImagePoolSize.descriptorCount)
			poolSizes.emplace_back(storageImagePoolSize);
		if (storageTexelBufferPoolSize.descriptorCount)
			poolSizes.emplace_back(storageTexelBufferPoolSize);

		// Sampled Images (such as: sampler2D)
		VkDescriptorPoolSize sampledImagePoolSize{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0 };
		for (const spirv_cross::Resource& res : resources.sampled_images)
		{
			unsigned binding = compiler.get_decoration(res.id, spv::DecorationBinding);

			sampledImagePoolSize.descriptorCount++;
			layoutBuilder.AddBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, binding, 1);
		}

		if (sampledImagePoolSize.descriptorCount)
			poolSizes.emplace_back(sampledImagePoolSize);

		// Atomic Counters
		// These are GLSL specialized resources used in atomic operations, allowing 
		//	synchronized access to a shared count variable across shader invocations.
		//	Vulkan doesn't really support this directly, but you can get the
		//	same result by declaring an unsigned int in a storage buffer and using
		//	glsl's atomic functions.
		//	For now, this will be omitted
		//for (const spirv_cross::Resource& res : resources.atomic_counters)
		//{
		//
		//}

		// Push Constant Buffers (such as: layout(push_constant) uniform Push) 
		//	(Vulkan only supports one push constant per shader stage)
		// VkPushConstantRange in VkPipelineLayoutCreateInfo
		for (const spirv_cross::Resource& res : resources.push_constant_buffers)
		{
			const auto& type = compiler.get_type(res.type_id);

			VkPushConstantRange range{};
			range.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT; // Alter to be shader specific
			range.offset = 0;
			range.size = static_cast<uint32_t>(compiler.get_declared_struct_size(type));

			pcRanges.emplace_back(range);
		}

		// Separate Images
		VkDescriptorPoolSize separateImagePoolSize{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 0 };
		VkDescriptorPoolSize separateBufferPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 0 };
		for (const spirv_cross::Resource& res : resources.separate_images)
		{
			const auto& type = compiler.get_type(res.type_id);
			unsigned binding = compiler.get_decoration(res.id, spv::DecorationBinding);

			if (type.image.dim == spv::DimBuffer)
			{
				separateBufferPoolSize.descriptorCount++;
				layoutBuilder.AddBinding(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, binding, 1);
			}
			else
			{
				separateImagePoolSize.descriptorCount++;
				layoutBuilder.AddBinding(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, binding, 1);
			}
		}

		if (separateImagePoolSize.descriptorCount)
			poolSizes.emplace_back(separateImagePoolSize);
		if (separateBufferPoolSize.descriptorCount)
			poolSizes.emplace_back(separateBufferPoolSize);

		// Separate Samplers (such as: sampler/samplerShadow)
		VkDescriptorPoolSize separateSamplerImagePoolSize{ VK_DESCRIPTOR_TYPE_SAMPLER, 0 };
		for (const spirv_cross::Resource& res : resources.separate_samplers)
		{
			separateSamplerImagePoolSize.descriptorCount++;

			unsigned binding = compiler.get_decoration(res.id, spv::DecorationBinding);
			layoutBuilder.AddBinding(VK_DESCRIPTOR_TYPE_SAMPLER, binding, 1);
		}

		if (separateSamplerImagePoolSize.descriptorCount)
			poolSizes.emplace_back(separateSamplerImagePoolSize);

		// Shader Record Buffers are to be used with Vulkan Raytracing.
		//	These are managed through Vulkan's shader binding table (SBT) instead of
		//	descriptor sets.
		// Raytracing hasn't been integrated yet, so it will be left omitted.
		//for (const spirv_cross::Resource& res : resources.shader_record_buffers)
		//{
		//
		//}

		// Acceleration Structures
		// Similar to the Shader Record Buffers, these are used with Vulkan Raytracing.
		//	The correspondingd descriptor type is VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR
		// Raytracing hasn't been integrated yet, so it will be left omitted.
		//for (const spirv_cross::Resource& res : resources.acceleration_structures)
		//{
		//
		//}

		// The remaining fields of spirv_cross::ShaderResources are for built-in,
		//	shader-specific variables that are mainly useful for understanding the
		//	shader, and perhaps some reconstruction. (builtin_inputs, builtin_outputs)
	}

	VulkanPipelineBuilder::VulkanPipelineBuilder(const VkDevice& device)
		: m_LogicalDevice(device)
	{
		m_Shaders.resize(INDY_PIPELINE_SHADER_STAGE_MAX_ENUM);
		m_ShaderModules.resize(INDY_PIPELINE_SHADER_STAGE_MAX_ENUM);
	}

	VulkanPipelineBuilder::~VulkanPipelineBuilder()
	{
		Clear();
	}

	// Vulkan Pipeline Builder Class Definitions

	void VulkanPipelineBuilder::Build(PipelineBuildOptions* options)
	{
		switch (options->type)
		{
			case INDY_PIPELINE_TYPE_COMPUTE:	BuildComputePipeline(); break;
			case INDY_PIPELINE_TYPE_GRAPHICS:	BuildGraphicsPipeline(static_cast<VulkanPipelineBuildOptions*>(options));  break;
			case INDY_PIPELINE_TYPE_RAYTRACE:	break;
		}
	}

	void VulkanPipelineBuilder::Clear()
	{
		m_Shaders.clear();

		for (const auto& module : m_ShaderModules)
			vkDestroyShaderModule(m_LogicalDevice, module, nullptr);

		m_ShaderModules.clear();

		m_Pipeline = {};
		m_Shaders.resize(INDY_PIPELINE_SHADER_STAGE_MAX_ENUM);
		m_ShaderModules.resize(INDY_PIPELINE_SHADER_STAGE_MAX_ENUM);
	}

	void VulkanPipelineBuilder::BindShader(const std::string& shaderPath)
	{
		// TODO:
		//	Deduce file name from file extension and search for cached SPIR-V file.
		//	Otherwise, read file and cache SPIR-V for next app load

		VulkanShader shader{};
		ReadShader(shaderPath, &shader);
		shader.spv = CompileSPIRVFromGLSL(shader);

		// Create Shader Module
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = shader.spv.size() * sizeof(uint32_t);
		createInfo.pCode = shader.spv.data();

		if (vkCreateShaderModule(m_LogicalDevice, &createInfo, nullptr, &m_ShaderModules[shader.type]) != VK_SUCCESS)
			INDY_CORE_ERROR("failed to create shader module!");

		// Store Shader
		m_Shaders[shader.type] = shader;
	}

	void VulkanPipelineBuilder::BindShader(const Shader& shader, const PipelineShaderStage& stage)
	{
		VulkanShader vShader(shader);

		// Compile to SPIR-V
		vShader.spv = CompileSPIRVFromGLSL(vShader);

		// Create Shader Module
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = vShader.spv.size() * sizeof(uint32_t);
		createInfo.pCode = vShader.spv.data();

		if (vkCreateShaderModule(m_LogicalDevice, &createInfo, nullptr, &m_ShaderModules[stage]) != VK_SUCCESS)
			INDY_CORE_ERROR("failed to create shader module!");

		// Store Shader
		m_Shaders[stage] = vShader;
	}

	void VulkanPipelineBuilder::BuildComputePipeline()
	{
		VulkanShader computeShader = m_Shaders[INDY_PIPELINE_SHADER_STAGE_COMPUTE];
		VulkanDescriptorSetBuilder layoutBuilder;
		std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
		std::vector<VkPushConstantRange> pushConstantRanges;

		ReflectVulkanShader(computeShader, layoutBuilder, descriptorPoolSizes, pushConstantRanges);

		// Create Descriptor Pool
		VkDescriptorPoolCreateInfo poolCreateInfo{};
		poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolCreateInfo.flags = 0;
		poolCreateInfo.maxSets = g_Max_Frames_In_Flight;
		poolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
		poolCreateInfo.pPoolSizes = descriptorPoolSizes.data();

		if (vkCreateDescriptorPool(m_LogicalDevice, &poolCreateInfo, nullptr, &m_Pipeline.descriptorPool) != VK_SUCCESS) {
			INDY_CORE_ERROR("Failed to create compute descriptor pool!");
			return;
		}

		// Build Descriptor Set Layout
		m_Pipeline.descriptorSetLayout = layoutBuilder.Build(m_LogicalDevice, VK_SHADER_STAGE_COMPUTE_BIT);

		// Create Pipeline Layout
		VkPipelineLayoutCreateInfo layoutCreateInfo{};
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutCreateInfo.setLayoutCount = 1;
		layoutCreateInfo.pSetLayouts = &m_Pipeline.descriptorSetLayout;
		layoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
		layoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();

		if (vkCreatePipelineLayout(m_LogicalDevice, &layoutCreateInfo, nullptr, &m_Pipeline.layout) != VK_SUCCESS) {
			INDY_CORE_ERROR("Failed to create compute pipeline layout!");
			return;
		}

		// Compute Pipeline Stage Setup
		VkPipelineShaderStageCreateInfo stageInfo{};
		stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageInfo.pNext = nullptr;
		stageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		stageInfo.module = m_ShaderModules[computeShader.type];
		stageInfo.pName = "main";

		// Compute Pipeline Creation
		VkComputePipelineCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.layout = m_Pipeline.layout;
		createInfo.stage = stageInfo;

		if (vkCreateComputePipelines(m_LogicalDevice, VK_NULL_HANDLE, 1, &createInfo, nullptr, &m_Pipeline.pipeline) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create Compute Pipeline!");
			return;
		}
	}

	void VulkanPipelineBuilder::BuildGraphicsPipeline(VulkanPipelineBuildOptions* options)
	{
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
		VkPipelineRasterizationStateCreateInfo rasterizerState{};
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		VkPipelineMultisampleStateCreateInfo multisamplingState{};
		VkPipelineDepthStencilStateCreateInfo depthStencilState{};
		VkPipelineRenderingCreateInfo renderInfo{};
		VkFormat colorAttachmentformat{};

		inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		rasterizerState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		multisamplingState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		renderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;

		// Assign shader modules
		VkShaderModule vertexModule = m_ShaderModules[INDY_PIPELINE_SHADER_STAGE_VERTEX];
		VkShaderModule fragmentModule = m_ShaderModules[INDY_PIPELINE_SHADER_STAGE_FRAGMENT];

		VkPipelineShaderStageCreateInfo vertexShaderStage{};
		vertexShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertexShaderStage.pNext = nullptr;
		vertexShaderStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertexShaderStage.module = vertexModule;
		vertexShaderStage.pName = "main";

		VkPipelineShaderStageCreateInfo fragmentShaderStage{};
		fragmentShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragmentShaderStage.pNext = nullptr;
		fragmentShaderStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragmentShaderStage.module = fragmentModule;
		fragmentShaderStage.pName = "main";

		shaderStages.emplace_back(vertexShaderStage);
		shaderStages.emplace_back(fragmentShaderStage);

		// Set up viewport state. Doesn't support multiple viewports yet.
		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.pNext = nullptr;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		// Dummy Color Blend State. No transparent objects yet.
		// If we need to render more than one color attachment, this can easily be extended.
		VkPipelineColorBlendStateCreateInfo colorBlendState{};
		colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendState.pNext = nullptr;
		colorBlendState.logicOpEnable = VK_FALSE;
		colorBlendState.logicOp = VK_LOGIC_OP_COPY;
		colorBlendState.attachmentCount = 1;
		colorBlendState.pAttachments = &colorBlendAttachment;

		// Input Assembly State
		inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyState.primitiveRestartEnable = VK_FALSE;

		// Rasterizer state
		rasterizerState.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizerState.lineWidth = 1.f;
		rasterizerState.cullMode = VK_CULL_MODE_NONE; // No culling
		rasterizerState.frontFace = VK_FRONT_FACE_CLOCKWISE; // Render vertices clockwise

		// Multisampling State. Default for now (none)
		multisamplingState.sampleShadingEnable = VK_FALSE;
		multisamplingState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; // no multisampling (1 sample per pixel)
		multisamplingState.minSampleShading = 1.0f;
		multisamplingState.pSampleMask = nullptr;
		multisamplingState.alphaToCoverageEnable = VK_FALSE; // no alpha to coverage either
		multisamplingState.alphaToOneEnable = VK_FALSE;

		// Color Blending (Disabled for now)
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		// Color Attachment Format
		colorAttachmentformat = VK_FORMAT_B8G8R8A8_UNORM; // This is temporary. Test for correct values

		// render Info color attachment
		renderInfo.colorAttachmentCount = 1;
		renderInfo.pColorAttachmentFormats = &colorAttachmentformat;

		// Render Info Depth Format
		renderInfo.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
		
		// Disable Depth Test
		depthStencilState.depthTestEnable = VK_FALSE;
		depthStencilState.depthWriteEnable = VK_FALSE;
		depthStencilState.depthCompareOp = VK_COMPARE_OP_NEVER;
		depthStencilState.depthBoundsTestEnable = VK_FALSE;
		depthStencilState.stencilTestEnable = VK_FALSE;
		depthStencilState.front = {};
		depthStencilState.back = {};
		depthStencilState.minDepthBounds = 0.f;
		depthStencilState.maxDepthBounds = 1.f;

		// Cleared Vertex Input State. This engine will use a single vertex buffer and index via shaders.
		VkPipelineVertexInputStateCreateInfo vertexInputState{};
		vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		// Shader Reflection
		VulkanShader vertexShader = m_Shaders[INDY_PIPELINE_SHADER_STAGE_VERTEX];
		VulkanShader fragmentShader = m_Shaders[INDY_PIPELINE_SHADER_STAGE_FRAGMENT];

		VulkanDescriptorSetBuilder layoutBuilder;
		std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
		std::vector<VkPushConstantRange> pushConstantRanges;

		ReflectVulkanShader(vertexShader, layoutBuilder, descriptorPoolSizes, pushConstantRanges);
		ReflectVulkanShader(fragmentShader, layoutBuilder, descriptorPoolSizes, pushConstantRanges);

		// Create Descriptor Pool
		VkDescriptorPoolCreateInfo poolCreateInfo{};
		poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolCreateInfo.flags = 0;
		poolCreateInfo.maxSets = g_Max_Frames_In_Flight;
		poolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
		poolCreateInfo.pPoolSizes = descriptorPoolSizes.data();

		if (vkCreateDescriptorPool(m_LogicalDevice, &poolCreateInfo, nullptr, &m_Pipeline.descriptorPool) != VK_SUCCESS) {
			INDY_CORE_ERROR("Failed to create graphics descriptor pool!");
			return;
		}

		// Build Descriptor Set Layout
		m_Pipeline.descriptorSetLayout = layoutBuilder.Build(m_LogicalDevice, 0);

		// Create Pipeline Layout
		VkPipelineLayoutCreateInfo layoutCreateInfo{};
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutCreateInfo.setLayoutCount = 1;
		layoutCreateInfo.pSetLayouts = &m_Pipeline.descriptorSetLayout;
		layoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
		layoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();

		if (vkCreatePipelineLayout(m_LogicalDevice, &layoutCreateInfo, nullptr, &m_Pipeline.layout) != VK_SUCCESS) {
			INDY_CORE_ERROR("Failed to create graphics pipeline layout!");
			return;
		}

		// Pipeline Create Info Setup
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.pNext = &renderInfo; // Connect render info
		pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputState;
		pipelineInfo.pInputAssemblyState = &inputAssemblyState;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizerState;
		pipelineInfo.pMultisampleState = &multisamplingState;
		pipelineInfo.pColorBlendState = &colorBlendState;
		pipelineInfo.pDepthStencilState = &depthStencilState;
		pipelineInfo.layout = m_Pipeline.layout;
		
		// Setup dynamic state
		VkDynamicState dynamicState[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

		VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
		dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateInfo.dynamicStateCount = 2;
		dynamicStateInfo.pDynamicStates = dynamicState;

		pipelineInfo.pDynamicState = &dynamicStateInfo;

		// Build Pipeline
		if (vkCreateGraphicsPipelines(m_LogicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline.pipeline) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create Graphics Pipeline!");
			return;
		}
	}
}