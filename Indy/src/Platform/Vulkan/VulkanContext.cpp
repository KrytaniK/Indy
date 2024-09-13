#include <Engine/Core/LogMacros.h>

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

import Indy.Graphics;
import Indy.VulkanGraphics;

namespace Indy::Graphics
{
	VulkanContext::VulkanContext(const VkInstance& instance, const uint32_t& id, const std::string& alias, const VulkanDeviceConfig& deviceConfig)
		: m_ID(id), m_Alias(alias), m_Instance(instance)
	{
		if (instance == VK_NULL_HANDLE)
		{
			INDY_CORE_WARN("Creating a Vulkan Context without a VkInstance! A working instance of Vulkan is required for contexts to function correctly. Context configuration will NOT be saved!\n\nIf VkInstance = VK_NULL_HANDLE, the context must be registered through Graphics::AddContext(), and Vulkan MUST be initialized via Graphics::Init(Graphics::Driver::Vulkan)!");
			return;
		}

		INDY_CORE_INFO("Creating Vulkan Context!");

		m_Device = std::make_unique<VulkanDevice>(deviceConfig, m_Instance);
	}

	VulkanContext::~VulkanContext()
	{
		// Clear render passes BEFORE destroying this context
		m_RenderPasses.clear();

		INDY_CORE_INFO("Destroying Vulkan Context!");
	}

	VulkanRenderPass& VulkanContext::AddRenderPass(const std::string& alias)
	{
		size_t id = m_RenderPasses.size();

		m_RenderPasses.emplace_back(alias, static_cast<uint32_t>(id));

		return m_RenderPasses[id];
	}

	VulkanRenderPass& VulkanContext::GetRenderPass(const uint32_t& id)
	{
		for (VulkanRenderPass& pass : m_RenderPasses)
		{
			if (pass.GetID() == id)
				return pass;
		}

		throw std::runtime_error("Failed to get Render Pass: Invalid Render Pass ID!");
	}

	bool VulkanContext::SetActiveViewport(const uint32_t& id)
	{
		if (id >= m_Viewports.size())
			return false;

		m_ActiveViewportID = id;

		return true;
	}

	bool VulkanContext::SetActiveViewport(const std::string& alias)
	{
		for (const auto& viewport : m_Viewports)
		{
			if (viewport.alias == alias)
			{
				m_ActiveViewportID = viewport.id;
				return true;
			}
		}

		return false;
	}
}