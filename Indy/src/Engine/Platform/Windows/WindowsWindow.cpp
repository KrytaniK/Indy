#include "WindowsWindow.h"

#include "Engine/Core/Log.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	static void GLFWErrorCallback(int error, const char* description)
	{
		INDY_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	WindowsWindow::WindowsWindow(const WindowSpec& spec)
	{

		// Initialize GLFW
		int b_success = glfwInit();

		if (!b_success)
		{
			INDY_CORE_CRITICAL("Could not initialize GLFW!");
		}

		// GLFW Error Callback
		glfwSetErrorCallback(GLFWErrorCallback);

		// Create GLFW Window
		// Set glfwWindowHints if using openGL.
		m_GLFW_Window = glfwCreateWindow((int)spec.width, (int)spec.height, spec.title.c_str(), NULL, NULL);

		if (!m_GLFW_Window)
		{
			INDY_CORE_CRITICAL("Window creation failed");
		}

		glfwMakeContextCurrent(m_GLFW_Window);

		// ---------------------------
		// GLFW Window Event Callbacks
		// ---------------------------

		glfwSetWindowCloseCallback(m_GLFW_Window, [](GLFWwindow* window)
		{
			// Create an application close event
			Event event{"LayerContext","AppClose"};

			// Dispatch event into the event context
			Events::Dispatch(event);
		});

		glfwSetWindowSizeCallback(m_GLFW_Window, [](GLFWwindow* window, int width, int height)
			{
				Event event{"LayerContext","WindowResize"};
				Events::Dispatch(event);
			});

		glfwSetScrollCallback(m_GLFW_Window, [](GLFWwindow* window, double xoffset, double yoffset)
			{
				Event event{ "LayerContext","Scroll" };
				Events::Dispatch(event);
			});

		glfwSetWindowFocusCallback(m_GLFW_Window, [](GLFWwindow* window, int focused)
			{
				switch (focused)
				{
				case GLFW_TRUE:
				{
					Event event{ "LayerContext","WindowFocus" };
					Events::Dispatch(event);
					break;
				}
				default:
				{
					Event event{ "LayerContext","WindowLoseFocus" };
					Events::Dispatch(event);
					break;
				}
				}
			});

		glfwSetWindowPosCallback(m_GLFW_Window, [](GLFWwindow* window, int xpos, int ypos)
			{
				Event event{ "LayerContext","WindowMove" };
				Events::Dispatch(event);
			});

		// GLFW Mouse Input Event Callbacks
		glfwSetCursorPosCallback(m_GLFW_Window, [](GLFWwindow* window, double xpos, double ypos)
			{
				Event event{ "LayerContext","MouseMove" };
				Events::Dispatch(event);
			});

		glfwSetMouseButtonCallback(m_GLFW_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				Event event{ "LayerContext","MouseButton" };
				Events::Dispatch(event);
			});

		// GLFW Keyboard Input Events
		glfwSetKeyCallback(m_GLFW_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				Event event{ "LayerContext","Keyboard" };
				Events::Dispatch(event);
			});
	}

	WindowsWindow::~WindowsWindow()
	{
		if (m_GLFW_Window)
			glfwDestroyWindow(m_GLFW_Window);

		glfwTerminate();
	}

	void WindowsWindow::onUpdate()
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(m_GLFW_Window);
		glfwPollEvents();
	}
}