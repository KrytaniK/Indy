#include <Engine/Core/LogMacros.h>

#include <memory>
#include <vector>

import Indy.VulkanRenderer;
import Indy.Events;

namespace Indy
{
	VulkanRenderer::VulkanRenderer()
	{
		m_Vulkan_Backend = std::make_unique<VulkanBackend>();

		// Generate unique renderer id
		m_RendererID = 0; // TODO: Actually generate ID

		// Bind Render Commands
		m_Commands.emplace_back(std::make_unique<VkCommand_Clear>());
		m_Commands.emplace_back(std::make_unique<VkCommand_Draw>());
		m_Commands.emplace_back(std::make_unique<VkCommand_DrawIndexed>());
		m_Commands.emplace_back(std::make_unique<VkCommand_DrawIndirect>());
		m_Commands.emplace_back(std::make_unique<VkCommand_DrawInstanced>());

		// Listen for render commands
		Events<IRenderCommandEvent>::Subscribe<VulkanRenderer>(this, &VulkanRenderer::OnCommand);
	}

	VulkanRenderer::~VulkanRenderer()
	{

	}

	void VulkanRenderer::Render()
	{
		// Begin the render pass for render targets that received commands this frame
		for (VulkanRenderTarget& renderTarget : m_RenderTargets)
		{
			if (renderTarget.Prepared())
				renderTarget.BeginRenderPass();
		}

		// Process and record all received render commands
		VkCommandEvent vk_event;
		while (!m_CommandQueue.empty())
		{
			vk_event = m_CommandQueue.front();
			m_CommandQueue.pop();

			uint8_t command = static_cast<uint8_t>(vk_event.command);
			m_Commands[command]->Execute(*vk_event.context, &vk_event.args);
		}

		// End the render pass for render targets that received commands this frame
		for (VulkanRenderTarget& renderTarget : m_RenderTargets)
		{
			if (renderTarget.Prepared())
				renderTarget.EndRenderPass();
		}
	}

	uint32_t VulkanRenderer::GetRendererID()
	{
		return m_RendererID;
	}

	void VulkanRenderer::OnCommand(IRenderCommandEvent* event)
	{
		if (event->rendererID != m_RendererID)
			return;

		VkCommandEvent* vk_event = static_cast<VkCommandEvent*>(event);

		m_CommandQueue.emplace(*vk_event);
	}
}