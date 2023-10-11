#include "WindowsWindow.h"

#include "Engine/Core/Log.h"

// GLFW
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Engine
{
	static void GLFWErrorCallback(int error, const char* description)
	{
		INDY_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	WindowsWindow::WindowsWindow(const WindowSpec& spec)
	{
		// ---------------
		// Initialize GLFW
		// ---------------

		int b_success = glfwInit();

		if (!b_success)
		{
			INDY_CORE_CRITICAL("Could not initialize GLFW!");
		}

		glfwSetErrorCallback(GLFWErrorCallback);

		// ---------------
		// Window Creation
		// ---------------

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Don't create an openGL context.
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Temporary | Disable window resize.
		m_GLFW_Window = glfwCreateWindow((int)spec.width, (int)spec.height, spec.title.c_str(), NULL, NULL); // Create GLFW Window

		if (!m_GLFW_Window)
		{
			INDY_CORE_CRITICAL("Window creation failed");
		}

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

		// --------------------------
		// GLFW Mouse Event Callbacks
		// --------------------------

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

		// -----------------------------
		// GLFW Keyboard Event Callbacks
		// -----------------------------

		glfwSetKeyCallback(m_GLFW_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				Event event{ "LayerContext","Keyboard" };
				Events::Dispatch(event);
			});

		// ---------------------
		// Vulkan Initialization <-- This is temporary, will be moved.
		// ---------------------

		this->InitVulkan();
	}

	WindowsWindow::~WindowsWindow()
	{
		vkDestroyInstance(m_VulkanInstance, nullptr); // Temp, will be moved
		
		if (m_GLFW_Window)
			glfwDestroyWindow(m_GLFW_Window);

		glfwTerminate();
	}

	void WindowsWindow::onUpdate()
	{
		glfwPollEvents();
	}

	void WindowsWindow::InitVulkan()
	{
		// App Info
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Indy Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		// Instance Creation Info
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		// Getting Platform Specific Extensions
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;

		// global validation layers
		createInfo.enabledLayerCount = 0;

		if (vkCreateInstance(&createInfo, nullptr, &m_VulkanInstance) != VK_SUCCESS) {
			INDY_CORE_ERROR("failed to create instance!");
		}
	}
}