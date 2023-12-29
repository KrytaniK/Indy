#include "GLFWWindowAPI.h"

#include "Engine/Input/Input.h"

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
		if (RenderContext::Get() == RENDER_API_VULKAN)
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
		if (RenderContext::Get() != RENDER_API_VULKAN)
		{
			glfwMakeContextCurrent(m_Window);
		}

		// GLFW FramebufferResize Callback
		glfwSetFramebufferSizeCallback(m_Window, 
			[](GLFWwindow* window, int width, int height) 
			{
				// Dispatch a window resize event to the Render event context.
				Event event{"Render", "Resize"};
				Events::Dispatch(event);
			}
		);

		// ---------------------------
		// GLFW Window Event Callbacks
		// ---------------------------

		glfwSetWindowCloseCallback(m_Window, 
			[](GLFWwindow* window)
			{
				// Create an application close event
				Event event{ "Window","Close" };

				// Dispatch event into the event context
				Events::Dispatch(event);
			}
		);

		glfwSetWindowSizeCallback(m_Window, 
			[](GLFWwindow* window, int width, int height)
			{
				Event event{ "Window","Resize" };
				Events::Dispatch(event);
			}
		);

		glfwSetWindowFocusCallback(m_Window, 
			[](GLFWwindow* window, int focused)
			{
				switch (focused)
				{
				case GLFW_TRUE:
				{
					Event event{ "Window","Focus" };
					Events::Dispatch(event);
					break;
				}
				default:
				{
					Event event{ "Window","LoseFocus" };
					Events::Dispatch(event);
					break;
				}
				}
			}
		);

		glfwSetWindowPosCallback(m_Window, 
			[](GLFWwindow* window, int xpos, int ypos)
			{
				Event event{ "Window","Move" };
				Events::Dispatch(event);
			}
		);

		// --------------------------
		// GLFW Mouse Event Callbacks
		// --------------------------

		glfwSetScrollCallback(m_Window,
			[](GLFWwindow* window, double xoffset, double yoffset)
			{
				Event inputEvent{"Input", "Raw"};

				InputDescription desc;
				desc.deviceInput.deviceID = InputManager::GetDeviceID("Mouse");
				desc.deviceInput.layoutID = InputManager::GetDeviceLayoutID("Mouse", "Scroll");
				desc.deviceInput.type = InputTypes::Scroll;

				std::vector<double> input = { xoffset, yoffset };

				desc.data = input.data();

				inputEvent.data = &desc;

				Events::Dispatch(inputEvent);
			}
		);

		glfwSetCursorPosCallback(m_Window, 
			[](GLFWwindow* window, double xpos, double ypos)
			{
				Event inputEvent{ "Input", "Raw" };

				InputDescription desc;
				desc.deviceInput.deviceID = InputManager::GetDeviceID("Mouse");
				desc.deviceInput.layoutID = InputManager::GetDeviceLayoutID("Mouse", "Position");
				desc.deviceInput.type = InputTypes::Axis2D;

				std::vector<double> input = { xpos, ypos };

				desc.data = input.data();

				inputEvent.data = &desc;

				Events::Dispatch(inputEvent);
			}
		);

		glfwSetMouseButtonCallback(m_Window, 
			[](GLFWwindow* window, int button, int action, int mods)
			{
				Event inputEvent{ "Input", "Raw" };

				InputDescription desc;
				desc.deviceInput.deviceID = InputManager::GetDeviceID("Mouse");
				desc.deviceInput.layoutID = InputManager::GetDeviceLayoutID("Mouse", "Button");
				desc.deviceInput.type = InputTypes::Button;

				std::vector<int> input = {button, action, mods};
				 
				desc.data = input.data();

				inputEvent.data = &desc;

				Events::Dispatch(inputEvent);
			}
		);

		// -----------------------------
		// GLFW Keyboard Event Callbacks
		// -----------------------------

		glfwSetKeyCallback(m_Window, 
			[](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				Event inputEvent{ "Input", "Raw" };

				InputDescription desc;
				desc.deviceInput.deviceID = InputManager::GetDeviceID("Keyboard");
				desc.deviceInput.layoutID = InputManager::GetDeviceLayoutID("Keyboard", "Key");
				desc.deviceInput.type = InputTypes::Button;

				std::vector<int> input = { key, scancode, action, mods };

				desc.data = input.data();

				inputEvent.data = &desc;

				Events::Dispatch(inputEvent);
			}
		);
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