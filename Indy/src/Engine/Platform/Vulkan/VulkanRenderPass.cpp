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

	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		
	}

	const VulkanRenderPass& VulkanRenderPass::Begin(const std::string& alias)
	{
		return *this;
	}

	const VulkanRenderPass& VulkanRenderPass::Begin(const uint32_t& id)
	{
		return *this;
	}

	void VulkanRenderPass::End()
	{

	}

	const VulkanRenderPass& VulkanRenderPass::AddPreProcessShader(const std::string& shaderPath)
	{
		return *this;
	}

	const VulkanRenderPass& VulkanRenderPass::BindVertexShader(const std::string& shaderPath)
	{
		return *this;
	}

	const VulkanRenderPass& VulkanRenderPass::BindTessellationControlShader(const std::string& shaderPath)
	{
		return *this;
	}

	const VulkanRenderPass& VulkanRenderPass::BindTessellationEvalShader(const std::string& shaderPath)
	{
		return *this;
	}

	const VulkanRenderPass& VulkanRenderPass::BindGeometryShader(const std::string& shaderPath)
	{
		return *this;
	}

	const VulkanRenderPass& VulkanRenderPass::BindFragmentShader(const std::string& shaderPath)
	{
		return *this;
	}

	const VulkanRenderPass& VulkanRenderPass::AddPostProcessShader(const std::string& shaderPath)
	{
		return *this;
	}

	const VulkanRenderPass& VulkanRenderPass::SetTopology(const PrimitiveTopology& topology)
	{
		return *this;
	}
	const VulkanRenderPass& VulkanRenderPass::SetPolygonMode(const PolygonMode& mode)
	{
		return *this;
	}
	const VulkanRenderPass& VulkanRenderPass::SetFrontFace(const FrontFace& frontFace)
	{
		return *this;
	}
	const VulkanRenderPass& VulkanRenderPass::SetBlendState(const bool enabled, const std::vector<ColorComponent>& masks)
	{
		return *this;
	}
	const VulkanRenderPass& VulkanRenderPass::EnableDepthTesting(const bool enabled)
	{
		return *this;
	}
	const VulkanRenderPass& VulkanRenderPass::EnableDepthWriting(const bool enabled)
	{
		return *this;
	}
}