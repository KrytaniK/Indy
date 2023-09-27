#include "WindowsWindow.h"
#include "WindowsEvents.h"

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

		// GLFW Event Callbacks
		glfwSetWindowCloseCallback(m_GLFW_Window, [](GLFWwindow* window)
		{
			WindowCloseEvent event{ window };
			Events::Dispatch<WindowCloseEvent>(event);
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
		Events::Bind<WindowCloseEvent>([](const WindowCloseEvent& event) { glfwDestroyWindow(event.window); });
	}

	void WindowsWindow::onUpdate()
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(m_GLFW_Window);
		glfwPollEvents();
	}
}