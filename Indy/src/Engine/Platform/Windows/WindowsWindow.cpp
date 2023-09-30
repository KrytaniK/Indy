#include "WindowsWindow.h"

#include "Engine/Core/Log.h"

namespace Engine
{
	static void GLFWErrorCallback(int error, const char* description)
	{
		INDY_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	WindowsWindow::WindowsWindow(const WindowSpec& spec)
	{
		this->BindApplicationEvents();

		// Initialize GLFW
		int b_success = glfwInit();

		if (!b_success)
		{
			INDY_CORE_CRITICAL("Could not initialize GLFW!");
		}

		// GLFW Error Callback
		glfwSetErrorCallback(GLFWErrorCallback);

		// Create GLFW Window
		// An ifdef could be used to set glfwWindowHints for use with openGL.
		m_GLFW_Window = glfwCreateWindow((int)spec.width, (int)spec.height, spec.title.c_str(), NULL, NULL);

		if (!m_GLFW_Window)
		{
			INDY_CORE_CRITICAL("Window creation failed");
		}

		glfwMakeContextCurrent(m_GLFW_Window);

		// GLFW Window Event Callbacks
		glfwSetWindowCloseCallback(m_GLFW_Window, [](GLFWwindow* window)
		{
			WindowCloseEvent event{ window, true };
			Events::Dispatch<WindowCloseEvent>(event);
		});

		glfwSetWindowSizeCallback(m_GLFW_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowResizeEvent event{ window, width, height };
			Events::Dispatch<WindowResizeEvent>(event);
		});

		glfwSetScrollCallback(m_GLFW_Window, [](GLFWwindow* window, double xoffset, double yoffset) 
		{
			ScrollEvent event{ window, xoffset, yoffset };
			Events::Dispatch<ScrollEvent>(event);
		});

		glfwSetWindowFocusCallback(m_GLFW_Window, [](GLFWwindow* window, int focused)
		{
			switch (focused)
			{
				case GLFW_TRUE:
				{
					WindowFocusEvent focusEvent{ window };
					Events::Dispatch<WindowFocusEvent>(focusEvent);
					break;
				}
				default:
				{
					WindowLoseFocusEvent loseFocusEvent{ window };
					Events::Dispatch<WindowLoseFocusEvent>(loseFocusEvent);
					break;
				}
			}
		});

		glfwSetWindowPosCallback(m_GLFW_Window, [](GLFWwindow* window, int xpos, int ypos) 
		{
			WindowMoveEvent event{window, xpos, ypos};
			Events::Dispatch<WindowMoveEvent>(event);
		});

		// GLFW Mouse Input Event Callbacks
		glfwSetCursorPosCallback(m_GLFW_Window, [](GLFWwindow* window, double xpos, double ypos)
		{
			MouseMoveEvent event{window, xpos, ypos};
			Events::Dispatch<MouseMoveEvent>(event);
		});

		glfwSetMouseButtonCallback(m_GLFW_Window, [](GLFWwindow* window, int button, int action, int mods) 
		{
			MouseButtonEvent event{ window, button, action, mods };
			Events::Dispatch<MouseButtonEvent>(event);
		});

		// GLFW Keyboard Input Events
		glfwSetKeyCallback(m_GLFW_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			KeyboardEvent event{window, key, scancode, action, mods};
			Events::Dispatch<KeyboardEvent>(event);
		});

	}

	WindowsWindow::~WindowsWindow()
	{
		for (EventHandle& handle : m_eventHandles)
		{
			Events::UnBind(handle);
		}

		if (m_GLFW_Window)
			glfwDestroyWindow(m_GLFW_Window);

		glfwTerminate();
	}

	void WindowsWindow::BindApplicationEvents()
	{
		// Window Related Callbacks
		m_eventHandles.emplace_back(Events::Bind<WindowCloseEvent>(this, &WindowsWindow::onWindowClose));
		m_eventHandles.emplace_back(Events::Bind<WindowResizeEvent>(this, &WindowsWindow::onWindowResize));
		m_eventHandles.emplace_back(Events::Bind<WindowMoveEvent>(this, &WindowsWindow::onWindowMove));
		m_eventHandles.emplace_back(Events::Bind<WindowFocusEvent>(this, &WindowsWindow::onWindowFocus));
		m_eventHandles.emplace_back(Events::Bind<WindowLoseFocusEvent>(this, &WindowsWindow::onWindowLoseFocus));

		/*	Note:
				Input related events will later be passed on some sort of input handler class. The window class should not be responsible for input.
		*/

		// General Input Callbacks
		m_eventHandles.emplace_back(Events::Bind<ScrollEvent>([](const ScrollEvent& event) { INDY_CORE_TRACE("[Scroll Event]: xOffset: {0}, yOffset: {1}", event.xoffset, event.yoffset); }));

		// Keyboard Input Callbacks
		m_eventHandles.emplace_back(Events::Bind<KeyboardEvent>([](const KeyboardEvent& event) { INDY_CORE_TRACE("[Key Event]: key: {0}, scancode: {1}, action: {2}, mods: {3}", event.key, event.scancode, event.action, event.mods); }));

		// Mouse Input Callbacks
		m_eventHandles.emplace_back(Events::Bind<MouseMoveEvent>([](const MouseMoveEvent& event) { INDY_CORE_TRACE("[Mouse Move Event]: x: {0}, y: {1}", event.xpos, event.ypos); }));
		m_eventHandles.emplace_back(Events::Bind<MouseButtonEvent>([](const MouseButtonEvent& event) 
		{ 
			INDY_CORE_TRACE("[Mouse Button Event]: Button: {0}, Action: {1}, Mods: {2}", event.button, event.action, event.mods); 
		}));
	}

	void WindowsWindow::onUpdate()
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(m_GLFW_Window);
		glfwPollEvents();
	}

	//  -------------
	//  Event Handles
	//  -------------

	void WindowsWindow::onWindowClose(const WindowCloseEvent& event) 
	{
		INDY_CORE_WARN("[Window Close Event]: Closing...");
		glfwDestroyWindow(event.window);
	};

	void WindowsWindow::onWindowResize(const WindowResizeEvent& event)
	{
		INDY_CORE_TRACE("[Window Resize Event]: newWidth: {0}, newHeight: {1}", event.width, event.height);
		//glfwSetWindowSize(event.window, event.width, event.height);
	};

	void WindowsWindow::onWindowMove(const WindowMoveEvent& event)
	{
		INDY_CORE_TRACE("[Window Move Event]: newX: {0}, newY: {1}", event.xpos, event.ypos);
	};

	void WindowsWindow::onWindowFocus(const WindowFocusEvent& event)
	{
		INDY_CORE_WARN("[Window Focus Event]");
	};

	void WindowsWindow::onWindowLoseFocus(const WindowLoseFocusEvent& event)
	{
		INDY_CORE_WARN("[Window Lose Focus Event]");
	};
}