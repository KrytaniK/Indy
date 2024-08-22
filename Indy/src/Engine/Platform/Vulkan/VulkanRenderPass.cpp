#include <Engine/Core/LogMacros.h>

#include <cstdint>
#include <string>
#include <vector>

import Indy.Graphics;
import Indy.VulkanGraphics;

namespace Indy::Graphics
{
	VulkanRenderPass::VulkanRenderPass(const std::string& alias, const uint32_t& id)
	{
		INDY_CORE_INFO("Creating a Vulkan Render Pass!");
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		INDY_CORE_INFO("Destroying a Vulkan Render Pass!");
	}

	VulkanRenderPass& VulkanRenderPass::Begin()
	{
		return *this;
	}

	void VulkanRenderPass::End()
	{

	}

	VulkanRenderPass& VulkanRenderPass::AddPreProcessShader(const std::string& shaderPath)
	{
		return *this;
	}

	VulkanRenderPass& VulkanRenderPass::BindVertexShader(const std::string& shaderPath)
	{
		return *this;
	}

	VulkanRenderPass& VulkanRenderPass::BindTessellationControlShader(const std::string& shaderPath)
	{
		return *this;
	}

	VulkanRenderPass& VulkanRenderPass::BindTessellationEvalShader(const std::string& shaderPath)
	{
		return *this;
	}

	VulkanRenderPass& VulkanRenderPass::BindGeometryShader(const std::string& shaderPath)
	{
		return *this;
	}

	VulkanRenderPass& VulkanRenderPass::BindFragmentShader(const std::string& shaderPath)
	{
		return *this;
	}

	VulkanRenderPass& VulkanRenderPass::AddPostProcessShader(const std::string& shaderPath)
	{
		return *this;
	}

	VulkanRenderPass& VulkanRenderPass::SetTopology(const PrimitiveTopology& topology)
	{
		return *this;
	}

	VulkanRenderPass& VulkanRenderPass::SetPolygonMode(const PolygonMode& mode)
	{
		return *this;
	}

	VulkanRenderPass& VulkanRenderPass::SetFrontFace(const FrontFace& frontFace)
	{
		return *this;
	}

	VulkanRenderPass& VulkanRenderPass::SetBlendState(const bool enabled, const std::vector<ColorComponent>& masks)
	{
		return *this;
	}

	VulkanRenderPass& VulkanRenderPass::EnableDepthTesting(const bool enabled)
	{
		return *this;
	}
	
	VulkanRenderPass& VulkanRenderPass::EnableDepthWriting(const bool enabled)
	{
		return *this;
	}
}