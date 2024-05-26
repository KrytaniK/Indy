#include "Engine/Core/LogMacros.h"

#include <memory>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

import Indy.Profiler;
import Indy.VulkanGraphics;
import Indy.Events;

namespace Indy::Graphics
{
	VulkanRenderTarget::VulkanRenderTarget(const VkInstance& instance, const VKDeviceCompat& compatibility, IWindow* window)
	{
		m_Instance = instance;

		if (!window)
			return;

		INDY_CORE_TRACE("Creating Surface");
		// Create VkSurface
		if (glfwCreateWindowSurface(m_Instance, static_cast<GLFWwindow*>(window->NativeWindow()), nullptr, &m_Surface) != VK_SUCCESS)
		{
			INDY_CORE_CRITICAL("Failed to create window surface!");
			return;
		}

		m_Device = std::make_unique<VulkanDevice>(compatibility, m_Surface);
		m_Swapchain = std::make_unique<VulkanSwapchain>(*m_Device->GetPhysicalDevice(), m_Device->Get(), m_Surface, window);
	}

	VulkanRenderTarget::~VulkanRenderTarget()
	{
		m_Swapchain = nullptr; // explicitly destruct swap chain BEFORE surface.

		if (&m_Surface)
		{
			INDY_CORE_TRACE("Destroying Window Surface!");
			vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		}
	}
}