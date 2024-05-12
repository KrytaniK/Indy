#include <Engine/Core/LogMacros.h>

#include <memory>


import Indy.VulkanRenderer;
import Indy.Window;
import Indy.Events;

namespace Indy
{
	VulkanRenderer::VulkanRenderer()
	{
		// Generate Window Data
		ICL_WindowData_Create createInfo;
		createInfo.title = "Test Window";
		createInfo.width = 1200;
		createInfo.height = 760;
		createInfo.id = 0;

		// Generate Window Create Event
		ICL_WindowEvent windowCreateEvent;
		windowCreateEvent.targetLayer = "ICL_Window";
		windowCreateEvent.action = WindowLayerAction::OpenWindow;
		windowCreateEvent.layerData = &createInfo;

		// Dispatch Window Create Event
		Events<ILayerEvent>::Notify(&windowCreateEvent);

		// Retrieve Window Handle
		if (!createInfo.handle || createInfo.handle->window.expired())
		{
			INDY_CORE_ERROR("Failed to initialized Vulkan Renderer. Invalid window handle.");
			return;
		}
		else
		{
			m_WindowHandle = createInfo.handle;
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
		// Bind Render() to window's onUpdate event handler.
		m_WindowHandle->window.lock()->onUpdate.Subscribe<VulkanRenderer>(this, &VulkanRenderer::Render);

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