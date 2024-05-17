#include <Engine/Core/LogMacros.h>
#include <vulkan/vulkan.h>

import Indy.VulkanRenderer;

namespace Indy
{
	void VulkanRenderTarget::Initialize()
	{
		m_Handle.instance = VulkanBackend::GetInstance();

		if (!m_Handle.instance)
		{
			INDY_CORE_ERROR("Failed to create Render Target. Vulkan is not initialized!");
			return;
		}
		
		m_Handle.renderTarget = this;
	}

	void VulkanRenderTarget::PrepareRenderPass()
	{
		if (m_Prepared)
			return;

		m_Prepared = true;
	}

	void VulkanRenderTarget::BeginRenderPass()
	{
		
	}

	void VulkanRenderTarget::EndRenderPass()
	{
		
	}

	VulkanContext* VulkanRenderTarget::GetHandle()
	{
		return &m_Handle;
	}

	uint32_t VulkanRenderTarget::GetWidth()
	{
		return 0;
	}

	uint32_t VulkanRenderTarget::GetHeight()
	{
		return 0;
	}

	bool VulkanRenderTarget::Prepared()
	{
		return m_Prepared;
	}

}