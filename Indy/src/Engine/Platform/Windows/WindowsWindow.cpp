#include "WindowsWindow.h"

#include "Engine/Core/Log.h"
#include "Engine/Layers/LayerEventContext.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	static void GLFWErrorCallback(int error, const char* description)
	{
		INDY_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	WindowsWindow::WindowsWindow(const WindowSpec& spec)
	{
		// NOTE :: GLFW Initialization should probably be done it its own scope, rather than within the window.
		/*	Stripping the GLFW-specific code to its own namespace (such as a GraphicsAPI), would clean this up a lot.
			struct GLFW_Spec
			{
				GraphicsAPI graphicsAPI; // This is specific, and defined per use case. The idea is that this is interchangable.
				WindowSpec& windowSpec;
				std::function<void(*)> errorCallback;

				// Window Callbacks
				GLFWwindowclosefun windowCloseCallback;
				GLFWwindowsizefun windowResizeCallback;
				GLFWscrollfun windowScrollCallback;
				GLFWwindowfocusfun windowFocusCallback;
				GLFWwindowposfun windowMoveCallback;

				// Keyboard Callbacks
				GLFWkeyfun keyboardCallback;

				// Mouse Callbacks
				GLFWcursorposfun mouseMoveCallback;
				GLFWmousebuttonfun mouseButtonCallback;
			}

			GLFW_Spec glfwSpec;

			m_GLFW_Window = GLFW_Init(glfwSpec);
		*/

		// It's also worth noting that the GLAD web service is down. Last known commit was on April 4, 2023
		/*
			To use GLAD, it is now required to clone the repo from https://github.com/Dav1dde/glad/tree/glad2
			- See Reddit Thread: https://www.reddit.com/r/opengl/comments/n2wo8y/glad_generator_seems_to_be_down/
			- Python is required to execute the CL args.
		*/

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

		using Events::EventType;

		glfwSetWindowCloseCallback(m_GLFW_Window, [](GLFWwindow* window)
		{
			WindowCloseEvent windowCloseEvent;
			windowCloseEvent.window = window;
			windowCloseEvent.type = EventType::WindowClose;
			windowCloseEvent.b_AppShouldTerminate = true;

			Events::Dispatch<LayerEventContext>(windowCloseEvent);
		});

		#ifdef THING
		glfwSetWindowSizeCallback(m_GLFW_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowResizeEvent event;
			event.type = EventType::WindowResize;
			event.window = window;
			event.width = width;
			event.height = heigth;

			Events::Dispatch<LayerEventContext>(event);
		});

		glfwSetScrollCallback(m_GLFW_Window, [](GLFWwindow* window, double xoffset, double yoffset) 
		{
			ScrollEvent event;
			event.type = EventType::Scroll;
			event.window = window;
			event.xoffset = xoffset;
			event.yoffset = yoffset;
			Events::Dispatch<LayerEventContext>(event);
		});

		glfwSetWindowFocusCallback(m_GLFW_Window, [](GLFWwindow* window, int focused)
		{
			switch (focused)
			{
				case GLFW_TRUE:
				{
					WindowFocusEvent focusEvent;
					focusEvent.type = EventType::WindowFocus;
					focusEvent.window = window;

					Events::Dispatch<LayerEventContext>(focusEvent);
					break;
				}
				default:
				{
					WindowLoseFocusEvent loseFocusEvent;
					focusEvent.type = EventType::WindowLoseFocus;
					focusEvent.window = window;

					Events::Dispatch<LayerEventContext>(loseFocusEvent);
					break;
				}
			}
		});

		glfwSetWindowPosCallback(m_GLFW_Window, [](GLFWwindow* window, int xpos, int ypos) 
		{
			WindowMoveEvent event;
			event.type = EventType::WindowMove;
			event.window = window;
			event.xpos = xpos;
			event.ypos = ypos;

			Events::Dispatch<LayerEventContext>(event);
		});

		// GLFW Mouse Input Event Callbacks
		glfwSetCursorPosCallback(m_GLFW_Window, [](GLFWwindow* window, double xpos, double ypos)
		{
			MouseMoveEvent event;
			event.type = EventType::MouseMove;
			event.window = window;
			event.xpos = xpos;
			event.ypos = ypos;

			Events::Dispatch<LayerEventContext>(event);
		});

		glfwSetMouseButtonCallback(m_GLFW_Window, [](GLFWwindow* window, int button, int action, int mods) 
		{
			MouseButtonEvent event;
			event.type = EventType::MouseButton;
			event.window = window;
			event.button = button;
			event.action = action;
			event.mods = mods;

			Events::Dispatch<LayerEventContext>(event);
		});

		// GLFW Keyboard Input Events
		glfwSetKeyCallback(m_GLFW_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			KeyboardEvent event{ EventType::Keyboard, window, key, scancode, action, mods };
			event.type = EventType::Keyboard;
			event.window = window;
			event.key = key;
			event.scancode = scancode;
			event.action = action;
			event.mods = mods;

			Events::Dispatch<LayerEventContext>(event);
		});
		#endif
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