#include "Engine/Core/LogMacros.h"

#include <utility>
#include <GLFW/glfw3.h>

import Indy_Core_Window;
import Indy_Core_Events;
import Indy_Core_InputSystem;

namespace Indy
{
	WindowsWindow::WindowsWindow(const WindowCreateInfo& createInfo)
	{
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		m_NativeWindow = glfwCreateWindow(createInfo.width, createInfo.height, createInfo.title.c_str(), NULL, NULL);

		if (m_NativeWindow == nullptr)
		{
			INDY_CORE_CRITICAL("[WindowsWindow] GLFW Window creation failed!");
			return;
		}

		// ------------------------------------------------------------------------
		// Native Input Setup -----------------------------------------------------
		// ------------------------------------------------------------------------

		m_InputManager = std::make_unique<InputManager>();
		m_GFLWWindowPtr = std::make_pair(&m_Props, m_InputManager.get());

		// ------------------------------------------------------------------------
		// GLFW Window Event Callbacks --------------------------------------------
		// ------------------------------------------------------------------------

		glfwSetWindowUserPointer(m_NativeWindow, &m_GFLWWindowPtr);

		// GLFW FramebufferResize Callback
		glfwSetFramebufferSizeCallback(m_NativeWindow, [](GLFWwindow* window, int width, int height){});

		glfwSetWindowCloseCallback(m_NativeWindow, [](GLFWwindow* window)
			{
				auto ptr = static_cast<std::pair<WindowProps*, InputManager*>*>(glfwGetWindowUserPointer(window));

				WindowDestroyEvent event{};
				event.id = ptr->first->id;

				EventManager::Notify<WindowDestroyEvent>(event);
			}
		);

		glfwSetWindowSizeCallback(m_NativeWindow, [](GLFWwindow* window, int width, int height)
			{
				auto ptr = static_cast<std::pair<WindowProps*, InputManager*>*>(glfwGetWindowUserPointer(window));

				ptr->first->width = width;
				ptr->first->height = height;
			}
		);

		glfwSetWindowFocusCallback(m_NativeWindow, [](GLFWwindow* window, int focused)
			{
				auto ptr = static_cast<std::pair<WindowProps*, InputManager*>*>(glfwGetWindowUserPointer(window));

				ptr->first->focused = focused;

				if (focused == GLFW_FALSE)
					ptr->second->Disable();
				else
					ptr->second->Enable();
			}
		);

		glfwSetWindowPosCallback(m_NativeWindow, [](GLFWwindow* window, int xpos, int ypos){});

		// -----------------------------------------------------------------------
		// GLFW Mouse Event Callbacks --------------------------------------------
		// -----------------------------------------------------------------------

		glfwSetScrollCallback(m_NativeWindow, [](GLFWwindow* window, double xoffset, double yoffset)
			{
				
			}
		);

		glfwSetCursorPosCallback(m_NativeWindow, [](GLFWwindow* window, double xpos, double ypos)
			{
				
			}
		);

		glfwSetMouseButtonCallback(m_NativeWindow, [](GLFWwindow* window, int button, int action, int mods)
			{
				
			}
		);

		// --------------------------------------------------------------------------
		// GLFW Keyboard Event Callbacks --------------------------------------------
		// --------------------------------------------------------------------------

		glfwSetKeyCallback(m_NativeWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				// Repeat actions aren't necessary (for now)
				if (action == GLFW_REPEAT)
					return;

				std::string targetControl;
				switch (key)
				{
					case GLFW_KEY_SPACE:
					{
						targetControl = "Space";
						break;
					}
					default:
					{
						targetControl = "Space";
						break;
					}
				}

				const InputDeviceControlInfo& ctrlInfo = InputManager::GetDeviceControlInfo("Default Keyboard", targetControl);

				InputDeviceEvent event;
				event.deviceFormat = ctrlInfo.deviceFormat;
				event.offset = ctrlInfo.offset;
				event.size = ctrlInfo.Size();
				event.bit = ctrlInfo.bit;
				event.deviceState = &action;

				EventManager::Notify<InputDeviceEvent>(event);
			}
		);
	}

	WindowsWindow::~WindowsWindow()
	{
		glfwSetWindowUserPointer(m_NativeWindow, nullptr);

		if (m_NativeWindow)
			glfwDestroyWindow(m_NativeWindow);
	}

	void WindowsWindow::Update()
	{
		// GLFW Window Input Events
		glfwPollEvents();

		// Indy Input Processing
		m_InputManager->ProcessEvents();
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