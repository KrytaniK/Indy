#include "Engine/Core/LogMacros.h"

#include <GLFW/glfw3.h>

import Indy_Core_Window;
import Indy_Core_EventSystem;

namespace Indy
{
	WindowsWindow::WindowsWindow(const WindowCreateInfo& createInfo)
		: m_Props({createInfo.title, createInfo.width, createInfo.height , createInfo.id})
	{
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		m_NativeWindow = glfwCreateWindow(createInfo.width, createInfo.height, createInfo.title.c_str(), NULL, NULL);

		if (m_NativeWindow == nullptr)
		{
			INDY_CORE_CRITICAL("[WindowsWindow] GLFW Window creation failed!");
			return;
		}

		glfwSetWindowUserPointer(m_NativeWindow, &m_Props);

		// ------------------------------------------------------------------------
		// GLFW Window Event Callbacks --------------------------------------------
		// ------------------------------------------------------------------------

		// GLFW FramebufferResize Callback
		glfwSetFramebufferSizeCallback(m_NativeWindow, [](GLFWwindow* window, int width, int height){});

		glfwSetWindowCloseCallback(m_NativeWindow, [](GLFWwindow* window)
			{
				WindowProps* props = static_cast<WindowProps*>(glfwGetWindowUserPointer(window));
				WindowDestroyEvent event{};
				event.id = props->id;
				EventManager::Notify<WindowDestroyEvent>(&event);
			}
		);

		glfwSetWindowSizeCallback(m_NativeWindow, [](GLFWwindow* window, int width, int height)
			{
				WindowProps* props = static_cast<WindowProps*>(glfwGetWindowUserPointer(window));
				props->width = width;
				props->height = height;
			}
		);

		glfwSetWindowFocusCallback(m_NativeWindow, [](GLFWwindow* window, int focused)
			{
				WindowProps* props = static_cast<WindowProps*>(glfwGetWindowUserPointer(window));
				props->focused = focused;
			}
		);

		glfwSetWindowPosCallback(m_NativeWindow, [](GLFWwindow* window, int xpos, int ypos){});

		// -----------------------------------------------------------------------
		// GLFW Mouse Event Callbacks --------------------------------------------
		// -----------------------------------------------------------------------

		glfwSetScrollCallback(m_NativeWindow, [](GLFWwindow* window, double xoffset, double yoffset){});

		glfwSetCursorPosCallback(m_NativeWindow, [](GLFWwindow* window, double xpos, double ypos){});

		glfwSetMouseButtonCallback(m_NativeWindow, [](GLFWwindow* window, int button, int action, int mods){});

		// --------------------------------------------------------------------------
		// GLFW Keyboard Event Callbacks --------------------------------------------
		// --------------------------------------------------------------------------

		glfwSetKeyCallback(m_NativeWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods){});
	}

	WindowsWindow::~WindowsWindow()
	{
		glfwSetWindowUserPointer(m_NativeWindow, nullptr);

		if (m_NativeWindow)
			glfwDestroyWindow(m_NativeWindow);
	}

	void WindowsWindow::Update()
	{
		glfwPollEvents();
	}

	void* WindowsWindow::NativeWindow() const
	{
		return m_NativeWindow;
	}

	const WindowProps& WindowsWindow::Properties() const
	{
		return m_Props;
	}
}