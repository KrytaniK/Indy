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

	VulkanPipelineBuilder::VulkanPipelineBuilder(const VkDevice& device)
		: m_LogicalDevice(device)
	{
		m_Shaders.resize(INDY_PIPELINE_SHADER_STAGE_MAX_ENUM);
		m_ShaderModules.resize(INDY_PIPELINE_SHADER_STAGE_MAX_ENUM);
	}

	// Vulkan Pipeline Builder Class Definitions

	void VulkanPipelineBuilder::Build(PipelineBuildOptions* options)
	{
		switch (options->type)
		{
			case INDY_PIPELINE_TYPE_COMPUTE:	BuildComputePipeline(); break;
			case INDY_PIPELINE_TYPE_GRAPHICS:	break;
			case INDY_PIPELINE_TYPE_RAYTRACE:	break;
		}
	}

	void VulkanPipelineBuilder::Clear()
	{
		m_Shaders.clear();
		m_ShaderModules.clear();
	}

	void VulkanPipelineBuilder::BindShader(const std::string& shaderPath)
	{
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

		// Compile to spir-v
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

		spirv_cross::Compiler compiler(computeShader.spv);

		// Reflect compute shader and retrieve resources
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		INDY_CORE_INFO("Checking Uniform Buffers");

		// Uniform Buffers (such as: uniform UBO)
		// VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER or VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
		for (const spirv_cross::Resource& res : resources.uniform_buffers)
		{
			unsigned set = compiler.get_decoration(res.id, spv::DecorationDescriptorSet);
			unsigned binding = compiler.get_decoration(res.id, spv::DecorationBinding);
			INDY_CORE_INFO("Descriptor Set: {0}\nBinding: {1}", set, binding);
		}

		INDY_CORE_INFO("Checking Storage Buffers");

		// Storage Buffers (such as: buffer SSBO)
		// VK_DESCRIPTOR_TYPE_STORAGE_BUFFER or VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC
		for (const spirv_cross::Resource& res : resources.storage_buffers)
		{
			unsigned set = compiler.get_decoration(res.id, spv::DecorationDescriptorSet);
			unsigned binding = compiler.get_decoration(res.id, spv::DecorationBinding);
			INDY_CORE_INFO("Descriptor Set: {0}\nBinding: {1}", set, binding);
		}

		INDY_CORE_INFO("Checking Stage Inputs");

		// Stage Inputs (such as: [in vec2 uv] in vertex shader)
		// VkVertexInputAttributeDescription
		for (const spirv_cross::Resource& res : resources.stage_inputs)
		{
			unsigned set = compiler.get_decoration(res.id, spv::DecorationDescriptorSet);
			unsigned binding = compiler.get_decoration(res.id, spv::DecorationBinding);
			INDY_CORE_INFO("Descriptor Set: {0}\nBinding: {1}", set, binding);
		}

		INDY_CORE_INFO("Checking Stage Outputs");

		// Stage Outputs (such as: [out vec4 FragColor] in fragment shader)
		// Maps to several things, but particularly useful to set write mask to 0 in 
		//	VkPipelineColorBlendAttachmentState if a location is not written to in shader.
		for (const spirv_cross::Resource& res : resources.stage_outputs)
		{
			unsigned set = compiler.get_decoration(res.id, spv::DecorationDescriptorSet);
			unsigned binding = compiler.get_decoration(res.id, spv::DecorationBinding);
			INDY_CORE_INFO("Descriptor Set: {0}\nBinding: {1}", set, binding);
		}

		INDY_CORE_INFO("Checking Subpass Inputs");

		// Subpass Inputs (such as: subpassInput)
		// VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
		for (const spirv_cross::Resource& res : resources.subpass_inputs)
		{
			unsigned set = compiler.get_decoration(res.id, spv::DecorationDescriptorSet);
			unsigned binding = compiler.get_decoration(res.id, spv::DecorationBinding);
			INDY_CORE_INFO("Descriptor Set: {0}\nBinding: {1}", set, binding);
		}

		INDY_CORE_INFO("Checking Storage Images");

		// Storage Images (such as: image2D)
		// 	Can vary:
		//		for image2D: VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
		//		for imageBuffer: VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER (Where storage_images (type.image.dim = DimBuffer))
		// VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
		for (const spirv_cross::Resource& res : resources.storage_images)
		{
			unsigned set = compiler.get_decoration(res.id, spv::DecorationDescriptorSet);
			unsigned binding = compiler.get_decoration(res.id, spv::DecorationBinding);
			INDY_CORE_INFO("Descriptor Set: {0}\nBinding: {1}", set, binding);
		}

		INDY_CORE_INFO("Checking Sampled Images");

		// Sampled Images (such as: sampler2D)
		// VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
		for (const spirv_cross::Resource& res : resources.sampled_images)
		{
			unsigned set = compiler.get_decoration(res.id, spv::DecorationDescriptorSet);
			unsigned binding = compiler.get_decoration(res.id, spv::DecorationBinding);
			INDY_CORE_INFO("Descriptor Set: {0}\nBinding: {1}", set, binding);
		}

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

		}

		// Separate Images
		//	Can vary:
		//		for texture2D: VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE
		//		for samplerBuffer: VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER (Where separate_images (type.image.dim = DimBuffer))
		// VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE
		for (const spirv_cross::Resource& res : resources.separate_images)
		{

		}

		// Separate Samplers (such as: sampler/samplerShadow)
		// VK_DESCRIPTOR_TYPE_SAMPLER
		for (const spirv_cross::Resource& res : resources.separate_samplers)
		{

		}

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

		// Deduce descriptor types, and counts

		// Create Descriptor Pool

		// Build Descriptor Set Layout
	}

	// ---------------------------------------------------------------------------------
	// OLD IMPL ------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------


	//VkShaderStageFlagBits VulkanPipeline::GetShaderStage(const ShaderType& shaderType)
	//{
	//	switch(shaderType)
	//	{
	//		case INDY_SHADER_TYPE_COMPUTE: return VK_SHADER_STAGE_COMPUTE_BIT;
	//		case INDY_SHADER_TYPE_VERTEX: return VK_SHADER_STAGE_VERTEX_BIT;
	//		case INDY_SHADER_TYPE_FRAGMENT: return VK_SHADER_STAGE_FRAGMENT_BIT;
	//		case INDY_SHADER_TYPE_TESS_CONTROL: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	//		case INDY_SHADER_TYPE_TESS_EVAL: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	//		default: return VK_SHADER_STAGE_ALL;
	//	}
	//}

	//VulkanPipeline::VulkanPipeline(const VkDevice& logicalDevice, const VulkanPipelineInfo& info)
	//	: m_Info(info), m_LogicalDevice(logicalDevice)
	//{
	//	// Resize to max value of shader type enum
	//	m_Descriptors.resize(6); 
	//	m_PushConstants.resize(6);
	//	m_ShaderModules.resize(6); 
	//}

	//VulkanPipeline::~VulkanPipeline()
	//{
	//	for (const auto& shaderModule : m_ShaderModules)
	//		vkDestroyShaderModule(m_LogicalDevice, shaderModule, nullptr);

	//	for (auto& descriptor : m_Descriptors)
	//		vkDestroyDescriptorSetLayout(m_LogicalDevice, descriptor->GetLayout(), nullptr);

	//	vkDestroyPipelineLayout(m_LogicalDevice, m_Info.layout, nullptr);
	//	vkDestroyPipeline(m_LogicalDevice, m_Info.pipeline, nullptr);
	//}

	//void VulkanPipeline::BindDescriptorSetLayout(const ShaderType& shaderType, const VulkanDescriptorPool& descriptorPool, const VkDescriptorSetLayout& layout)
	//{
	//	m_Descriptors[shaderType] = std::make_shared<VulkanDescriptor>(descriptorPool, layout);
	//}

	//void VulkanPipeline::BindPushConstants(const ShaderType& shaderType, const VkPushConstantRange& pushConstantRange)
	//{
	//	m_PushConstants[shaderType] = std::make_shared<VkPushConstantRange>(pushConstantRange);
	//}

	//void VulkanPipeline::Build()
	//{
	//	switch(m_Info.type)
	//	{
	//		case INDY_PIPELINE_TYPE_COMPUTE: { BuildComputePipeline(); return; }
	//		case INDY_PIPELINE_TYPE_RAYTRACE: { BuildRayTracePipeline(); return; }
	//		case INDY_PIPELINE_TYPE_GRAPHICS: { BuildGraphicsPipeline(); return; }
	//		default:
	//		{
	//			INDY_CORE_ERROR("Invalid Pipeline Type");
	//		}
	//	}
	//}

	//void VulkanPipeline::BuildComputePipeline() 
	//{
	//	// Get all layouts
	//	// -------------------------------------------------------------------------------------------------------

	//	std::vector<VkDescriptorSetLayout> layouts;
	//	for (std::shared_ptr<VulkanDescriptor>& descriptor : m_Descriptors)
	//		if (descriptor)
	//			layouts.emplace_back(descriptor->GetLayout());

	//	// Get all push constant ranges
	//	// -------------------------------------------------------------------------------------------------------
	//	std::vector<VkPushConstantRange> pushConstants;
	//	for (std::shared_ptr<VkPushConstantRange>& pushConstant : m_PushConstants)
	//		if (pushConstant)
	//			pushConstants.emplace_back(*pushConstant);
	//	

	//	// Build pipeline layout
	//	// -------------------------------------------------------------------------------------------------------

	//	VkPipelineLayoutCreateInfo createInfo{};
	//	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	//	createInfo.pNext = nullptr;
	//	createInfo.pSetLayouts = layouts.data();
	//	createInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
	//	createInfo.pPushConstantRanges = pushConstants.data();
	//	createInfo.pushConstantRangeCount = pushConstants.size();

	//	if (vkCreatePipelineLayout(m_LogicalDevice, &createInfo, nullptr, &m_Info.layout) != VK_SUCCESS)
	//	{
	//		INDY_CORE_ERROR("Could not create compute pipeline layout!");
	//		return;
	//	}

	//	// Build Pipeline
	//	// -------------------------------------------------------------------------------------------------------

	//	VkPipelineShaderStageCreateInfo stageInfo{};
	//	stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	//	stageInfo.pNext = nullptr;
	//	stageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	//	stageInfo.module = m_ShaderModules[INDY_SHADER_TYPE_COMPUTE];
	//	stageInfo.pName = "main";

	//	VkComputePipelineCreateInfo computePipelineCreateInfo{};
	//	computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	//	computePipelineCreateInfo.pNext = nullptr;
	//	computePipelineCreateInfo.layout = m_Info.layout;
	//	computePipelineCreateInfo.stage = stageInfo;

	//	if (vkCreateComputePipelines(m_LogicalDevice, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &m_Info.pipeline) != VK_SUCCESS)
	//	{
	//		INDY_CORE_ERROR("Failed to create compute pipeline!");
	//	}
	//}

	//void VulkanPipeline::BuildGraphicsPipeline()
	//{
	//	// Get all layouts
	//	// -------------------------------------------------------------------------------------------------------

	//	std::vector<VkDescriptorSetLayout> layouts;
	//	for (std::shared_ptr<VulkanDescriptor>& descriptor : m_Descriptors)
	//		if (descriptor)
	//			layouts.emplace_back(descriptor->GetLayout());

	//	// Build pipeline layout
	//	// -------------------------------------------------------------------------------------------------------

	//	VkPipelineLayoutCreateInfo createInfo{};
	//	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	//	createInfo.pNext = nullptr;
	//	createInfo.pSetLayouts = layouts.data();
	//	createInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());

	//	if (vkCreatePipelineLayout(m_LogicalDevice, &createInfo, nullptr, &m_Info.layout) != VK_SUCCESS)
	//	{
	//		INDY_CORE_ERROR("Could not create compute pipeline layout!");
	//		return;
	//	}

	//	// Build Pipeline
	//	// -------------------------------------------------------------------------------------------------------

	//	// TODO: Implement Graphics Pipeline!
	//}

	//void VulkanPipeline::BuildRayTracePipeline()
	//{
	//	// Get all layouts
	//	// -------------------------------------------------------------------------------------------------------

	//	std::vector<VkDescriptorSetLayout> layouts;
	//	for (std::shared_ptr<VulkanDescriptor>& descriptor : m_Descriptors)
	//		if (descriptor)
	//			layouts.emplace_back(descriptor->GetLayout());

	//	// Build pipeline layout
	//	// -------------------------------------------------------------------------------------------------------

	//	VkPipelineLayoutCreateInfo createInfo{};
	//	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	//	createInfo.pNext = nullptr;
	//	createInfo.pSetLayouts = layouts.data();
	//	createInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());

	//	if (vkCreatePipelineLayout(m_LogicalDevice, &createInfo, nullptr, &m_Info.layout) != VK_SUCCESS)
	//	{
	//		INDY_CORE_ERROR("Could not create compute pipeline layout!");
	//		return;
	//	}

	//	// Build Pipeline
	//	// -------------------------------------------------------------------------------------------------------

	//	// TODO: Implement Ray Tracing Pipeline!
	//}
}