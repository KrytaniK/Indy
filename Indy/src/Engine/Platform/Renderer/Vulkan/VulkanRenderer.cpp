#include "Engine/Core/LogMacros.h"

#include <memory>

import Indy_Core_Renderer;
import Indy_Core_WindowLayer;
import Indy_Core_Window;
import Indy_Core_Events;

namespace Indy
{
	VulkanRenderer::VulkanRenderer()
	{
		// Generate Window Data
		AD_WindowCreateInfo createInfo;
		createInfo.title = "Test Window";
		createInfo.width = 1200;
		createInfo.height = 760;
		createInfo.id = 0;

		// Generate Window Create Event
		WindowLayerEvent windowCreateEvent;
		windowCreateEvent.targetLayer = "ICL_Window";
		windowCreateEvent.action = WindowLayerAction::OpenWindow;
		windowCreateEvent.layerData = &createInfo;

		// Dispatch Window Create Event
		EventManagerCSR::Notify<ILayerEvent>(&windowCreateEvent);

		// Retrieve Window Handle
		if (!createInfo.windowHandle || createInfo.windowHandle->window.expired())
		{
			INDY_CORE_ERROR("Failed to initialized Vulkan Renderer. Invalid window handle.");
			return;
		}
		else
		{
			m_WindowHandle = createInfo.windowHandle;
		}
		
		// Initialize
		Init();
	}

	VulkanRenderer::VulkanRenderer(IWindowHandle* windowHandle)
	{
		if (!windowHandle || windowHandle->window.expired())
		{
			INDY_CORE_ERROR("Failed to initialized Vulkan Renderer. Invalid window handle.");
			return;
		}

		// Set Window Handle
		m_WindowHandle = windowHandle;

		// Initialize
		Init();
	}

	VulkanRenderer::~VulkanRenderer()
	{

	}

	void VulkanRenderer::Render()
	{
		// Don't render if the window is invalid or minimized
		if (m_WindowHandle->window.expired() || 
			m_WindowHandle->window.lock()->Properties().minimized
		)
			return;

		// Draw current frame
		DrawFrame();

		// Idle the logical device
		//vkDeviceWaitIdle(/* logical device*/);
	}

	void VulkanRenderer::Init()
	{
		// Attach renderer to window (effectively enabling the renderer)

		// Generate vkInstance

		// Generate the window surface

		// Generate Digital and Logical Devices

		// Generate Swap Chain

		// Generate Descriptor Pools for each pipeline type (e.g., Graphics, Compute, etc.)

		// Generate Framebuffers

		// Generate Command Pools for each pipeline type (e.g., Graphics, Compute, etc.)

		// Generate Sync Objects
	}

	void VulkanRenderer::DrawFrame()
	{
		// Wait for previous frame to finish

		// Get next swap chain image

		// Reset the current fence

		// -- UBO updates --

		// Record command buffers

		// Submit command buffers

		// Present swapchain image

		// Update frame count
	}
}