#include <Engine/Core/LogMacros.h>

#include <cstdint>
#include <string>
#include <vector>

import Indy.Graphics;
import Indy.VulkanGraphics;

namespace Indy::Graphics
{
	VulkanRenderPass::VulkanRenderPass(const std::string& alias, const uint32_t& id)
		: m_IsConfigurable(false)
	{
		INDY_CORE_INFO("Creating a Vulkan Render Pass!");
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		INDY_CORE_INFO("Destroying a Vulkan Render Pass!");
	}

	VulkanRenderPass& VulkanRenderPass::Begin()
	{
		m_IsConfigurable = true;
		return *this;
	}

	VulkanRenderPass& VulkanRenderPass::AddProcess(const ProcessType& type)
	{
		if (!m_IsConfigurable)
		{
			INDY_CORE_WARN("Failed to add process to Vulkan Render Pass: RenderPass is not in configurable state. To enable configuration, a call to Begin() is required.");
			return *this;
		}

		return *this;
	}

	VulkanRenderPass& VulkanRenderPass::BindShader(const PipelineShaderStage& shaderStage, const std::string& path)
	{
		if (!m_IsConfigurable)
		{
			INDY_CORE_WARN("Failed to bind shader to Vulkan Render Pass: RenderPass is not in configurable state. To enable configuration, a call to Begin() is required.");
			return *this;
		}

		return *this;
	}

	void VulkanRenderPass::End()
	{
		if (!m_IsConfigurable)
		{
			INDY_CORE_WARN("Failed to end Vulkan Render Pass configuration: RenderPass is not in configurable state. To enable configuration, a call to Begin() is required.");
			return;
		}

		m_IsConfigurable = false;

		// Build all relative vulkan pipelines
	}

	VulkanRenderPass& VulkanRenderPass::SetTopology(const PrimitiveTopology& topology)
	{
		if (!m_IsConfigurable)
		{
			INDY_CORE_WARN("Failed to set the topology: RenderPass is not in configurable state. To enable configuration, a call to Begin() is required.");
			return *this;
		}

		return *this;
	}

	VulkanRenderPass& VulkanRenderPass::SetPolygonMode(const PolygonMode& mode)
	{
		if (!m_IsConfigurable)
		{
			INDY_CORE_WARN("Failed to set the polygon mode: RenderPass is not in configurable state. To enable configuration, a call to Begin() is required.");
			return *this;
		}

		return *this;
	}

	VulkanRenderPass& VulkanRenderPass::SetFrontFace(const FrontFace& frontFace)
	{
		if (!m_IsConfigurable)
		{
			INDY_CORE_WARN("Failed to set the front face: RenderPass is not in configurable state. To enable configuration, a call to Begin() is required.");
			return *this;
		}

		return *this;
	}

	VulkanRenderPass& VulkanRenderPass::SetBlendState(const bool enabled, const std::vector<ColorComponent>& masks)
	{
		if (!m_IsConfigurable)
		{
			INDY_CORE_WARN("Failed to set the blend state: RenderPass is not in configurable state. To enable configuration, a call to Begin() is required.");
			return *this;
		}

		return *this;
	}

	VulkanRenderPass& VulkanRenderPass::EnableDepthTesting(const bool enabled)
	{
		if (!m_IsConfigurable)
		{
			INDY_CORE_WARN("Failed to set enabled depth testing: RenderPass is not in configurable state. To enable configuration, a call to Begin() is required.");
			return *this;
		}

		return *this;
	}
	
	VulkanRenderPass& VulkanRenderPass::EnableDepthWriting(const bool enabled)
	{
		if (!m_IsConfigurable)
		{
			INDY_CORE_WARN("Failed to set enable depth writing: RenderPass is not in configurable state. To enable configuration, a call to Begin() is required.");
			return *this;
		}

		return *this;
	}
}