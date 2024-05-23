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
	}

	VulkanRenderTarget::~VulkanRenderTarget()
	{
		if (&m_Surface)
			vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	}


}