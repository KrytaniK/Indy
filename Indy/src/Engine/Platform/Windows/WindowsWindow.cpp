#include <Engine/Core/LogMacros.h>

#include <GLFW/glfw3.h>

import Indy.WindowsWindow;
import Indy.Window;
import Indy.Input;
import Indy.Events;

namespace Indy
{
	WindowsWindow::WindowsWindow(const WindowCreateInfo& createInfo)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // When OpenGL support becomes available, this will have to be removed.

		// GLFW Window Initialization
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		m_NativeWindow = glfwCreateWindow(createInfo.width, createInfo.height, createInfo.title.c_str(), nullptr, nullptr);

		if (m_NativeWindow == nullptr)
		{
			INDY_CORE_CRITICAL("[WindowsWindow] GLFW Window creation failed!");
			return;
		}

		// Set window user pointer and callbacks
		glfwSetWindowUserPointer(m_NativeWindow, this);
		SetGLFWWindowCallbacks();

		// Attach window properties
		m_Props.title = createInfo.title;
		m_Props.width = createInfo.width;
		m_Props.height = createInfo.height;
		m_Props.id = createInfo.id;

		// Create Input Context and set it to be the active context
		m_InputContext = std::make_unique<Input::InputContext>();

		Input::SetContextEvent event;
		event.newContext = m_InputContext.get();
		Events<Input::SetContextEvent>::Notify(&event);

		// Create Renderer
		//m_Renderer = Renderer::Create();
	}

	WindowsWindow::~WindowsWindow()
	{
		// Regular destruction stuff

		if (!m_NativeWindow)
			return;

		// GLFW destruction stuff (if a valid window exists)

		glfwSetWindowUserPointer(m_NativeWindow, nullptr);

		if (m_NativeWindow)
			glfwDestroyWindow(m_NativeWindow);
	}

	void WindowsWindow::Update()
	{
		// Don't update if we weren't properly initialized
		if (!m_NativeWindow)
			return;

		// GLFW Window Input Events
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

	Input::InputContext* WindowsWindow::GetInputContext()
	{
		return m_InputContext.get();
	}

	void WindowsWindow::SetExtent(const int& width, const int& height)
	{
		m_Props.width = width;
		m_Props.height = height;
	}

	void WindowsWindow::SetFocus(bool isFocused)
	{
		m_Props.focused = isFocused;
	}

	void WindowsWindow::SetMinimized(bool isMinimized)
	{
		m_Props.minimized = isMinimized;
	}

	void WindowsWindow::SetGLFWWindowCallbacks()
	{
		// ------------------------------------------------------------------------
		// GLFW Window Event Callbacks --------------------------------------------
		// ------------------------------------------------------------------------

		// GLFW FramebufferResize Callback
		glfwSetFramebufferSizeCallback(m_NativeWindow, [](GLFWwindow* window, int width, int height)
			{

			}
		);

		glfwSetWindowCloseCallback(m_NativeWindow, [](GLFWwindow* window)
			{
				WindowsWindow* _this = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));

				if (_this->m_NativeWindow)
				{
					glfwSetWindowUserPointer(_this->m_NativeWindow, nullptr);
					glfwDestroyWindow(_this->m_NativeWindow);
					_this->m_NativeWindow = nullptr;
				}
			}
		);

		glfwSetWindowSizeCallback(m_NativeWindow, [](GLFWwindow* window, int width, int height)
			{
				WindowsWindow* _this = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));

				if (!_this)
					return;

				_this->SetExtent(width, height);
			}
		);

		glfwSetWindowFocusCallback(m_NativeWindow, [](GLFWwindow* window, int focused)
			{
				WindowsWindow* _this = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));

				if (!_this)
					return;

				if (focused)
				{
					Input::SetContextEvent event;
					event.newContext = _this->m_InputContext.get();
					
					Events<Input::SetContextEvent>::Notify(&event);
				}

				_this->SetFocus(focused);
			}
		);

		glfwSetWindowIconifyCallback(m_NativeWindow, [](GLFWwindow* window, int iconified)
			{
				WindowsWindow* _this = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));

				if (!_this)
					return;

				_this->SetMinimized((bool)iconified);
			}
		);

		glfwSetWindowPosCallback(m_NativeWindow, [](GLFWwindow* window, int xpos, int ypos)
			{
				/* TODO: Implement */
			}
		);

		// -----------------------------------------------------------------------
		// GLFW Mouse Event Callbacks --------------------------------------------
		// -----------------------------------------------------------------------

		glfwSetScrollCallback(m_NativeWindow, [](GLFWwindow* window, double xoffset, double yoffset)
			{
				double scroll[] = { xoffset, yoffset };

				Input::Event event;
				event.device_name = "GLFW Mouse";
				event.control_alias = "Scroll";
				event.data = &scroll;

				Events<Input::Event>::Notify(&event);
			}
		);

		glfwSetCursorPosCallback(m_NativeWindow, [](GLFWwindow* window, double xpos, double ypos)
			{
				double position[] = { xpos, ypos };

				Input::Event event;
				event.device_name = "GLFW Mouse";
				event.control_alias = "Position";
				event.data = &position;

				Events<Input::Event>::Notify(&event);

			}
		);

		glfwSetMouseButtonCallback(m_NativeWindow, [](GLFWwindow* window, int button, int action, int mods)
			{
				Input::Event event;
				event.device_name = "GLFW Mouse";
				event.control_alias = std::to_string(button);
				event.data = &action;

				Events<Input::Event>::Notify(&event);
			}
		);

		// --------------------------------------------------------------------------
		// GLFW Keyboard Event Callbacks --------------------------------------------
		// --------------------------------------------------------------------------

		glfwSetKeyCallback(m_NativeWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				Input::Event event;
				event.device_name = "GLFW Keyboard";
				event.control_alias = std::to_string(key);
				event.data = &action;

				Events<Input::Event>::Notify(&event);
			}
		);
	}
}