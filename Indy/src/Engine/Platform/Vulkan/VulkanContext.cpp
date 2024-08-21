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

	bool VulkanContext::AddRenderPass(const RenderPass* renderPass)
	{
		return false;
	}

	const VulkanRenderPass& VulkanContext::GetRenderPass(const uint32_t& id)
	{
		for (auto& pass : m_RenderPasses)
		{
			if (pass.GetID() == id)
				return pass;
		}

		throw std::runtime_error("Failed to get Render Pass: Invalid Render Pass ID!");
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