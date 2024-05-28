#include "Engine/Core/LogMacros.h"

#include <memory>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

import Indy.Profiler;
import Indy.Graphics;
import Indy.VulkanGraphics;
import Indy.Events;

namespace Indy
{
	VulkanRenderTarget::VulkanRenderTarget(const VkInstance& instance, const GPUCompatibility& compatibility, Window* window)
	{
		m_ID = window->Properties().id;
		m_Instance = instance;

		if (!window)
			return;

		// Create VkSurface
		if (glfwCreateWindowSurface(m_Instance, static_cast<GLFWwindow*>(window->NativeWindow()), nullptr, &m_Surface) != VK_SUCCESS)
		{
			INDY_CORE_CRITICAL("Failed to create window surface!");
			return;
		}

		m_Device = std::make_unique<VulkanDevice>(compatibility, m_Surface);
		m_Swapchain = std::make_unique<VulkanSwapchain>(m_Device->GetPhysicalDevice(), m_Device->Get(), m_Surface, window);
	}

	VulkanRenderTarget::~VulkanRenderTarget()
	{
		m_Swapchain = nullptr; // explicitly destroy swap chain BEFORE VkSurface.

		if (&m_Surface)
			vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	}
}