#include <Engine/Core/LogMacros.h>

#include <memory>

#include <vulkan/vulkan.h>

import Indy.Graphics;
import Indy.VulkanGraphics;

namespace Indy::Graphics
{
	VulkanContext::VulkanContext(const VkInstance& instance, const uint32_t& id, const std::string& alias)
		: m_ID(id), m_Alias(alias), m_Instance(instance)
	{
		if (instance == VK_NULL_HANDLE)
		{
			INDY_CORE_WARN("Creating a Vulkan Context without a VkInstance! A working instance of Vulkan is required for contexts to function correctly. Context configuration will NOT be saved!\n\nIf VkInstance = VK_NULL_HANDLE, the context must be registered through Graphics::AddContext(), and Vulkan MUST be initialized via Graphics::Init(Graphics::Driver::Vulkan)!");
			return;
		}

		// Create Vulkan Device
	}

	VulkanContext::VulkanContext(RenderContext* context, const VkInstance& instance)
		: m_Instance(instance)
	{
		VulkanContext* tempCtx = static_cast<VulkanContext*>(context);

		// Copy basic info
		m_Alias = tempCtx->m_Alias;
		m_ID = tempCtx->m_ID;

		// Copy Viewport and Render Pass info
		m_Viewports = tempCtx->m_Viewports;
		m_RenderPasses = tempCtx->m_RenderPasses;

		// Create Vulkan Device
	}

	VulkanContext::~VulkanContext()
	{
	}

	bool VulkanContext::AddRenderPass(const RenderPass& renderPass) const
	{
		return false;
	}

	const RenderContext& VulkanContext::BeginRenderPass(const std::string& debugName) const
	{
		return *this;
	}

	const RenderContext& VulkanContext::BeginRenderPass(const uint32_t& id) const
	{
		return *this;
	}

	void VulkanContext::EndRenderPass() const
	{
	}

	const RenderPass& VulkanContext::GetRenderPass(const uint32_t& id) const
	{
		return m_RenderPasses[id];
	}

	const RenderContext& VulkanContext::AddPreProcessShader(const std::string& shaderPath) const
	{
		return *this;
	}

	const RenderContext& VulkanContext::BindVertexShader(const std::string& shaderPath) const
	{
		return *this;
	}

	const RenderContext& VulkanContext::BindTessellationControlShader(const std::string& shaderPath) const
	{
		return *this;
	}

	const RenderContext& VulkanContext::BindTessellationEvalShader(const std::string& shaderPath) const
	{
		return *this;
	}

	const RenderContext& VulkanContext::BindGeometryShader(const std::string& shaderPath) const
	{
		return *this;
	}

	const RenderContext& VulkanContext::BindFragmentShader(const std::string& shaderPath) const
	{
		return *this;
	}

	const RenderContext& VulkanContext::AddPostProcessShader(const std::string& shaderPath) const
	{
		return *this;
	}

	const RenderContext& VulkanContext::SetTopology(const PrimitiveTopology& topology) const
	{
		return *this;
	}

	const RenderContext& VulkanContext::SetPolygonMode(const PolygonMode& mode) const
	{
		return *this;
	}

	const RenderContext& VulkanContext::SetFrontFace(const FrontFace& frontFace) const
	{
		return *this;
	}

	const RenderContext& VulkanContext::SetBlendState(const bool enabled, const std::vector<ColorComponent>& masks) const
	{
		return *this;
	}

	const RenderContext& VulkanContext::EnableDepthTesting(const bool enabled) const
	{
		return *this;
	}

	const RenderContext& VulkanContext::EnableDepthWriting(const bool enabled) const
	{
		return *this;
	}

	bool VulkanContext::SetActiveViewport(const uint32_t& id) const
	{
		return false;
	}

	bool VulkanContext::SetActiveViewport(const std::string& alias) const
	{
		return false;
	}
}