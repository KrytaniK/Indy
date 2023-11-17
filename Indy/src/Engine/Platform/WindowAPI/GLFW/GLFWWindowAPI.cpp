#include "GLFWWindowAPI.h"

namespace Engine
{
	static void GLFWErrorCallback(int error, const char* description)
	{
		INDY_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	GLFW_WindowAPI::GLFW_WindowAPI(const WindowSpec& windowSpec)
	{
		int b_success = glfwInit();

		if (!b_success)
		{
			INDY_CORE_CRITICAL("Could not initialize GLFW!");
		}

		glfwSetErrorCallback(GLFWErrorCallback);

		// ---------------
		// Window Creation
		// ---------------

		// Vulkan doesn't need an OpenGL Context.
		if (RendererAPI::Get() == RENDERER_API_VULKAN)
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // No OpenGL Context is needed.
		}

		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_Window = glfwCreateWindow((int)windowSpec.width, (int)windowSpec.height, windowSpec.title.c_str(), NULL, NULL); // Create GLFW Window

		if (!m_Window)
		{
			INDY_CORE_CRITICAL("Window creation failed");
		}

		// Set OpenGL Context, if not using Vulkan
		if (RendererAPI::Get() != RENDERER_API_VULKAN)
		{
			glfwMakeContextCurrent(m_Window);
		}

		// GLFW FramebufferResize Callback
		glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) 
			{
				// Dispatch a window resize event to the Render event context.
				Event event{"RenderContext", "WindowResize"};
				Events::Dispatch(event);
			});

		// ---------------------------
		// GLFW Window Event Callbacks
		// ---------------------------

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				// Create an application close event
				Event event{ "LayerContext","AppClose" };

				// Dispatch event into the event context
				Events::Dispatch(event);
			});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				Event event{ "LayerContext","WindowResize" };
				Events::Dispatch(event);
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset)
			{
				Event event{ "LayerContext","Scroll" };
				Events::Dispatch(event);
			});

		glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused)
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

		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int xpos, int ypos)
			{
				Event event{ "LayerContext","WindowMove" };
				Events::Dispatch(event);
			});

		// --------------------------
		// GLFW Mouse Event Callbacks
		// --------------------------

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
			{
				Event event{ "LayerContext","MouseMove" };
				Events::Dispatch(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				Event event{ "LayerContext","MouseButton" };
				Events::Dispatch(event);
			});

		// -----------------------------
		// GLFW Keyboard Event Callbacks
		// -----------------------------

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				Event event{ "LayerContext","Keyboard" };
				Events::Dispatch(event);
			});
	}

	GLFW_WindowAPI::~GLFW_WindowAPI()
	{
		if (m_Window)
			glfwDestroyWindow(m_Window);

		glfwTerminate();
	}

	void GLFW_WindowAPI::onUpdate()
	{
		glfwPollEvents();
	}
}