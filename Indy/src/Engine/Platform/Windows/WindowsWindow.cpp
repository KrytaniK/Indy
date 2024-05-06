#include "Engine/Core/LogMacros.h"

#include <GLFW/glfw3.h>

import Indy_Core_WindowLayer;
import Indy_Core_Window;
import Indy_Core_InputLayer;
import Indy_Core_Input;
import Indy_Core_Events;

namespace Indy
{
	WindowsWindow::WindowsWindow(const WindowCreateInfo& createInfo)
	{
		// GLFW Mouse ----------------------------------------------------------

		#pragma region GLFW Mouse Device Creation

		DeviceInfo glfwMouseInfo;
		glfwMouseInfo.displayName = "GLFW Mouse";
		glfwMouseInfo.deviceClass = 0x0000; // 0x0000 represents a "Pointer" device, such as a touchpad or a mouse. TODO: Implement a better way to handle this.
		glfwMouseInfo.layoutClass = 0x4D53; // 0x4D53 would represent the uint16_t conversion from "GLMS". TODO: Implement function for this.

		DeviceLayout glfwMouseLayout;
		glfwMouseLayout.displayName = "GLFW Mouse";
		glfwMouseLayout.deviceClass = 0x0000;
		glfwMouseLayout.layoutClass = 0x4D53;
		glfwMouseLayout.sizeInBytes = (sizeof(double) * 4) + 1;
		glfwMouseLayout.controls = {
			{std::to_string(GLFW_MOUSE_BUTTON_1), 0, 1, 0, 0, 0},
			{std::to_string(GLFW_MOUSE_BUTTON_2), 0, 1, 0, 1, 0},
			{std::to_string(GLFW_MOUSE_BUTTON_3), 0, 1, 0, 2, 0},
			{std::to_string(GLFW_MOUSE_BUTTON_4), 0, 1, 0, 3, 0},
			{std::to_string(GLFW_MOUSE_BUTTON_5), 0, 1, 0, 4, 0},
			{std::to_string(GLFW_MOUSE_BUTTON_6), 0, 1, 0, 5, 0},
			{std::to_string(GLFW_MOUSE_BUTTON_7), 0, 1, 0, 6, 0},
			{std::to_string(GLFW_MOUSE_BUTTON_8), 0, 1, 0, 7, 0},
			{"Mouse Position", sizeof(double) * 2, sizeof(double) * 2 * 8, 1, 0xFF, 2},
			{"Mouse X", sizeof(double), sizeof(double) * 8, 1, 0, 0},
			{"Mouse Y", sizeof(double), sizeof(double) * 8, 5, 0, 0},
			{"Mouse Scroll", sizeof(double) * 2, sizeof(double) * 2 * 8, 1, 0xFF, 2},
			{"Scroll X", sizeof(double), sizeof(double) * 8, 1, 0, 0},
			{"Scroll Y", sizeof(double), sizeof(double) * 8, 5, 0, 0},
		};

		AD_InputCreateLayoutInfo glfwCreateLayoutData;
		glfwCreateLayoutData.layout = &glfwMouseLayout;

		AD_InputCreateDeviceInfo glfwCreateDeviceData;
		glfwCreateDeviceData.deviceInfo = &glfwMouseInfo;

		InputLayerEvent glfwCreateLayoutEvent;
		glfwCreateLayoutEvent.targetLayer = "ICL_Input";
		glfwCreateLayoutEvent.action = InputLayerAction::CreateLayout;
		glfwCreateLayoutEvent.layerData = &glfwCreateLayoutData;

		InputLayerEvent glfwCreateDeviceEvent;
		glfwCreateDeviceEvent.targetLayer = "ICL_Input";
		glfwCreateDeviceEvent.action = InputLayerAction::CreateDevice;
		glfwCreateDeviceEvent.layerData = &glfwCreateDeviceData;

		EventManagerCSR::Notify<ILayerEvent>(&glfwCreateLayoutEvent);
		EventManagerCSR::Notify<ILayerEvent>(&glfwCreateDeviceEvent);

		#pragma endregion

		// GLFW Keyboard -------------------------------------------------------

		#pragma region GLFW Keyboard Device Creation




		DeviceInfo glfwKeyboardInfo;
		glfwKeyboardInfo.displayName = "GLFW Keyboard";
		glfwKeyboardInfo.deviceClass = 0x0001; // 0x0001 represents a "Keyboard" device, such as a touchpad or a mouse. TODO: Implement a better way to handle this.
		glfwKeyboardInfo.layoutClass = 0x4B42; // 0x474C would represent the uint16_t conversion from "GLFW". TODO: Implement function for this.

		DeviceLayout glfwKeyboardLayout;
		glfwKeyboardLayout.displayName = "GLFW Keyboard";
		glfwKeyboardLayout.deviceClass = 0x0001;
		glfwKeyboardLayout.layoutClass = 0x4B42;
		glfwKeyboardLayout.sizeInBytes = 15; // 119 keys, 15 bytes
		glfwKeyboardLayout.controls = {
			// Byte 1
			{std::to_string(GLFW_KEY_0),			0, 1, 0, 0, 0}, // 0-9 decimal numbers
			{std::to_string(GLFW_KEY_1),			0, 1, 0, 1, 0},
			{std::to_string(GLFW_KEY_2),			0, 1, 0, 2, 0},
			{std::to_string(GLFW_KEY_3),			0, 1, 0, 3, 0},
			{std::to_string(GLFW_KEY_4),			0, 1, 0, 4, 0},
			{std::to_string(GLFW_KEY_5),			0, 1, 0, 5, 0},
			{std::to_string(GLFW_KEY_6),			0, 1, 0, 6, 0},
			{std::to_string(GLFW_KEY_7),			0, 1, 0, 7, 0},

			// Byte 2
			{std::to_string(GLFW_KEY_8),			0, 1, 1, 0, 0},
			{std::to_string(GLFW_KEY_9),			0, 1, 1, 1, 0},

			{std::to_string(GLFW_KEY_A),			0, 1, 1, 2, 0}, // U.S. standard alphabet
			{std::to_string(GLFW_KEY_B),			0, 1, 1, 3, 0},
			{std::to_string(GLFW_KEY_C),			0, 1, 1, 4, 0},
			{std::to_string(GLFW_KEY_D),			0, 1, 1, 5, 0},
			{std::to_string(GLFW_KEY_E),			0, 1, 1, 6, 0},
			{std::to_string(GLFW_KEY_F),			0, 1, 1, 7, 0},

			// Byte 3
			{std::to_string(GLFW_KEY_G),			0, 1, 2, 0, 0},
			{std::to_string(GLFW_KEY_H),			0, 1, 2, 1, 0},
			{std::to_string(GLFW_KEY_I),			0, 1, 2, 2, 0},
			{std::to_string(GLFW_KEY_J),			0, 1, 2, 3, 0},
			{std::to_string(GLFW_KEY_K),			0, 1, 2, 4, 0},
			{std::to_string(GLFW_KEY_L),			0, 1, 2, 5, 0},
			{std::to_string(GLFW_KEY_M),			0, 1, 2, 6, 0},
			{std::to_string(GLFW_KEY_N),			0, 1, 2, 7, 0},
			
			// Byte 4,
			{std::to_string(GLFW_KEY_O),			0, 1, 3, 0, 0},
			{std::to_string(GLFW_KEY_P),			0, 1, 3, 1, 0},
			{std::to_string(GLFW_KEY_Q),			0, 1, 3, 2, 0},
			{std::to_string(GLFW_KEY_R),			0, 1, 3, 3, 0},
			{std::to_string(GLFW_KEY_S),			0, 1, 3, 4, 0},
			{std::to_string(GLFW_KEY_T),			0, 1, 3, 5, 0},
			{std::to_string(GLFW_KEY_U),			0, 1, 3, 6, 0},
			{std::to_string(GLFW_KEY_V),			0, 1, 3, 7, 0},

			// Byte 5
			{std::to_string(GLFW_KEY_W),			0, 1, 4, 0, 0},
			{std::to_string(GLFW_KEY_X),			0, 1, 4, 1, 0},
			{std::to_string(GLFW_KEY_Y),			0, 1, 4, 2, 0},
			{std::to_string(GLFW_KEY_Z),			0, 1, 4, 3, 0},

			{std::to_string(GLFW_KEY_F1),			0, 1, 4, 4, 0}, // Standard F0-25 keys
			{std::to_string(GLFW_KEY_F2),			0, 1, 4, 5, 0},
			{std::to_string(GLFW_KEY_F3),			0, 1, 4, 6, 0},
			{std::to_string(GLFW_KEY_F4),			0, 1, 4, 7, 0},

			// Byte 6
			{std::to_string(GLFW_KEY_F5),			0, 1, 5, 0, 0},
			{std::to_string(GLFW_KEY_F6),			0, 1, 5, 1, 0},
			{std::to_string(GLFW_KEY_F7),			0, 1, 5, 2, 0},
			{std::to_string(GLFW_KEY_F8),			0, 1, 5, 3, 0},
			{std::to_string(GLFW_KEY_F9),			0, 1, 5, 4, 0},
			{std::to_string(GLFW_KEY_F10),			0, 1, 5, 5, 0},
			{std::to_string(GLFW_KEY_F11),			0, 1, 5, 6, 0},
			{std::to_string(GLFW_KEY_F12),			0, 1, 5, 7, 0},

			// Byte 7
			{std::to_string(GLFW_KEY_F13),			0, 1, 6, 0, 0},
			{std::to_string(GLFW_KEY_F14),			0, 1, 6, 1, 0},
			{std::to_string(GLFW_KEY_F15),			0, 1, 6, 2, 0},
			{std::to_string(GLFW_KEY_F16),			0, 1, 6, 3, 0},
			{std::to_string(GLFW_KEY_F17),			0, 1, 6, 4, 0},
			{std::to_string(GLFW_KEY_F18),			0, 1, 6, 5, 0},
			{std::to_string(GLFW_KEY_F19),			0, 1, 6, 6, 0},
			{std::to_string(GLFW_KEY_F20),			0, 1, 6, 7, 0},

			// Byte 8
			{std::to_string(GLFW_KEY_F21),			0, 1, 7, 0, 0},
			{std::to_string(GLFW_KEY_F22),			0, 1, 7, 1, 0},
			{std::to_string(GLFW_KEY_F23),			0, 1, 7, 2, 0},
			{std::to_string(GLFW_KEY_F24),			0, 1, 7, 3, 0},
			{std::to_string(GLFW_KEY_F25),			0, 1, 7, 4, 0},

			{std::to_string(GLFW_KEY_KP_0),			0, 1, 7, 5, 0}, // GLFW KP keys
			{std::to_string(GLFW_KEY_KP_1),			0, 1, 7, 6, 0},
			{std::to_string(GLFW_KEY_KP_2),			0, 1, 7, 7, 0},

			// Byte 9
			{std::to_string(GLFW_KEY_KP_3),			0, 1, 8, 0, 0},
			{std::to_string(GLFW_KEY_KP_4),			0, 1, 8, 1, 0},
			{std::to_string(GLFW_KEY_KP_5),			0, 1, 8, 2, 0},
			{std::to_string(GLFW_KEY_KP_6),			0, 1, 8, 3, 0},
			{std::to_string(GLFW_KEY_KP_7),			0, 1, 8, 4, 0},
			{std::to_string(GLFW_KEY_KP_8),			0, 1, 8, 5, 0},
			{std::to_string(GLFW_KEY_KP_9),			0, 1, 8, 6, 0},
			{std::to_string(GLFW_KEY_KP_DECIMAL),	0, 1, 8, 7, 0},

			// Byte 10
			{std::to_string(GLFW_KEY_KP_DIVIDE),	0, 1, 9, 0, 0},
			{std::to_string(GLFW_KEY_KP_MULTIPLY),	0, 1, 9, 1, 0},
			{std::to_string(GLFW_KEY_KP_SUBTRACT),	0, 1, 9, 2, 0},
			{std::to_string(GLFW_KEY_KP_ADD),		0, 1, 9, 3, 0},
			{std::to_string(GLFW_KEY_KP_ENTER),		0, 1, 9, 4, 0},
			{std::to_string(GLFW_KEY_KP_EQUAL),		0, 1, 9, 5, 0},

			{std::to_string(GLFW_KEY_SPACE),		0, 1, 9, 6, 0}, // Other keys, ordered by their ASCII value
			{std::to_string(GLFW_KEY_APOSTROPHE),	0, 1, 9, 7, 0},

			// Byte 11
			{std::to_string(GLFW_KEY_COMMA),		0, 1, 10, 0, 0},
			{std::to_string(GLFW_KEY_MINUS),		0, 1, 10, 1, 0},
			{std::to_string(GLFW_KEY_PERIOD),		0, 1, 10, 2, 0},
			{std::to_string(GLFW_KEY_SLASH),		0, 1, 10, 3, 0},
			{std::to_string(GLFW_KEY_SEMICOLON),	0, 1, 10, 4, 0},
			{std::to_string(GLFW_KEY_EQUAL),		0, 1, 10, 5, 0},
			{std::to_string(GLFW_KEY_LEFT_BRACKET),	0, 1, 10, 6, 0},
			{std::to_string(GLFW_KEY_BACKSLASH),	0, 1, 10, 7, 0},

			// Byte 12
			{std::to_string(GLFW_KEY_RIGHT_BRACKET),0, 1, 11, 0, 0},
			{std::to_string(GLFW_KEY_GRAVE_ACCENT),	0, 1, 11, 1, 0},
			{std::to_string(GLFW_KEY_WORLD_1),		0, 1, 11, 2, 0},
			{std::to_string(GLFW_KEY_WORLD_2),		0, 1, 11, 3, 0},
			{std::to_string(GLFW_KEY_ESCAPE),		0, 1, 11, 4, 0},
			{std::to_string(GLFW_KEY_ENTER),		0, 1, 11, 5, 0},
			{std::to_string(GLFW_KEY_TAB),			0, 1, 11, 6, 0},
			{std::to_string(GLFW_KEY_BACKSPACE),	0, 1, 11, 7, 0},

			// Byte 13
			{std::to_string(GLFW_KEY_INSERT),		0, 1, 12, 0, 0},
			{std::to_string(GLFW_KEY_DELETE),		0, 1, 12, 1, 0},
			{std::to_string(GLFW_KEY_RIGHT),		0, 1, 12, 2, 0},
			{std::to_string(GLFW_KEY_LEFT),			0, 1, 12, 3, 0},
			{std::to_string(GLFW_KEY_DOWN),			0, 1, 12, 4, 0},
			{std::to_string(GLFW_KEY_PAGE_UP),		0, 1, 12, 5, 0},
			{std::to_string(GLFW_KEY_PAGE_DOWN),	0, 1, 12, 6, 0},
			{std::to_string(GLFW_KEY_HOME),			0, 1, 12, 7, 0},

			// Byte 14
			{std::to_string(GLFW_KEY_END),			0, 1, 13, 0, 0},
			{std::to_string(GLFW_KEY_CAPS_LOCK),	0, 1, 13, 1, 0},
			{std::to_string(GLFW_KEY_SCROLL_LOCK),	0, 1, 13, 2, 0},
			{std::to_string(GLFW_KEY_NUM_LOCK),		0, 1, 13, 3, 0},
			{std::to_string(GLFW_KEY_PRINT_SCREEN),	0, 1, 13, 4, 0},
			{std::to_string(GLFW_KEY_PAUSE),		0, 1, 13, 5, 0},
			{std::to_string(GLFW_KEY_LEFT_SHIFT),	0, 1, 13, 6, 0},
			{std::to_string(GLFW_KEY_LEFT_CONTROL),	0, 1, 13, 7, 0},

			// Byte 15
			{std::to_string(GLFW_KEY_LEFT_ALT),		0, 1, 14, 0, 0},
			{std::to_string(GLFW_KEY_LEFT_SUPER),	0, 1, 14, 1, 0},
			{std::to_string(GLFW_KEY_RIGHT_SHIFT),	0, 1, 14, 2, 0},
			{std::to_string(GLFW_KEY_RIGHT_CONTROL),0, 1, 14, 3, 0},
			{std::to_string(GLFW_KEY_RIGHT_ALT),	0, 1, 14, 4, 0},
			{std::to_string(GLFW_KEY_RIGHT_SUPER),	0, 1, 14, 5, 0},
			{std::to_string(GLFW_KEY_MENU),			0, 1, 14, 6, 0},
		};

		AD_InputCreateLayoutInfo glfwCreateKeyLayoutData;
		glfwCreateKeyLayoutData.layout = &glfwKeyboardLayout;

		AD_InputCreateDeviceInfo glfwCreateKeyboardData;
		glfwCreateKeyboardData.deviceInfo = &glfwKeyboardInfo;

		InputLayerEvent glfwCreateKeyLayoutEvent;
		glfwCreateKeyLayoutEvent.targetLayer = "ICL_Input";
		glfwCreateKeyLayoutEvent.action = InputLayerAction::CreateLayout;
		glfwCreateKeyLayoutEvent.layerData = &glfwCreateKeyLayoutData;

		InputLayerEvent glfwCreateKeyboardEvent;
		glfwCreateKeyboardEvent.targetLayer = "ICL_Input";
		glfwCreateKeyboardEvent.action = InputLayerAction::CreateDevice;
		glfwCreateKeyboardEvent.layerData = &glfwCreateKeyboardData;

		EventManagerCSR::Notify<ILayerEvent>(&glfwCreateKeyLayoutEvent);
		EventManagerCSR::Notify<ILayerEvent>(&glfwCreateKeyboardEvent);

		#pragma endregion

		// ---------------------------------------------------------------------
		// GLFW Native Window Setup --------------------------------------------
		// ---------------------------------------------------------------------

		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_NativeWindow = glfwCreateWindow(createInfo.width, createInfo.height, createInfo.title.c_str(), NULL, NULL);

		if (m_NativeWindow == nullptr)
		{
			INDY_CORE_CRITICAL("[WindowsWindow] GLFW Window creation failed!");
			return;
		}

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
				IWindowHandle* handle = static_cast<IWindowHandle*>(glfwGetWindowUserPointer(window));

				AD_WindowDestroyInfo destroyInfo;
				destroyInfo.index = handle->index; // This should be the index of this window.

				WindowLayerEvent closeEvent;
				closeEvent.targetLayer = "ICL_Window";
				closeEvent.action = WindowLayerAction::DestroyWindow;
				closeEvent.layerData = &destroyInfo;

				EventManagerCSR::Notify<ILayerEvent>(&closeEvent);
			}
		);

		glfwSetWindowSizeCallback(m_NativeWindow, [](GLFWwindow* window, int width, int height)
			{
				IWindowHandle* handle = static_cast<IWindowHandle*>(glfwGetWindowUserPointer(window));

				if (handle->window.expired())
					return;

				handle->window.lock()->SetExtent(width, height);
			}
		);

		glfwSetWindowFocusCallback(m_NativeWindow, [](GLFWwindow* window, int focused)
			{
				IWindowHandle* handle = static_cast<IWindowHandle*>(glfwGetWindowUserPointer(window));

				if (handle->window.expired())
					return;

				handle->window.lock()->SetFocus(focused);
			}
		);

		glfwSetWindowPosCallback(m_NativeWindow, [](GLFWwindow* window, int xpos, int ypos){});

		// -----------------------------------------------------------------------
		// GLFW Mouse Event Callbacks --------------------------------------------
		// -----------------------------------------------------------------------

		glfwSetScrollCallback(m_NativeWindow, [](GLFWwindow* window, double xoffset, double yoffset)
			{
				double scroll[] = { xoffset, yoffset };

				AD_InputUpdateInfo scrollData;
				scrollData.deviceClass = 0x0000; // "Pointer"
				scrollData.layoutClass = 0x4D53; // "GLFW Mouse"
				scrollData.control = "Mouse Scroll";
				scrollData.newState = &scroll;

				InputLayerEvent inputEvent;
				inputEvent.targetLayer = "ICL_Input";
				inputEvent.action = InputLayerAction::Update;
				inputEvent.layerData = &scrollData;

				EventManagerCSR::Notify<ILayerEvent>(&inputEvent);
			}
		);

		glfwSetCursorPosCallback(m_NativeWindow, [](GLFWwindow* window, double xpos, double ypos)
			{
				double position[] = { xpos, ypos };

				AD_InputUpdateInfo posData;
				posData.deviceClass = 0x0000; // "Pointer"
				posData.layoutClass = 0x4D53; // "GLFW Mouse"
				posData.control = "Mouse Position";
				posData.newState = &position;

				InputLayerEvent inputEvent;
				inputEvent.targetLayer = "ICL_Input";
				inputEvent.action = InputLayerAction::Update;
				inputEvent.layerData = &posData;

				EventManagerCSR::Notify<ILayerEvent>(&inputEvent);
			}
		);

		glfwSetMouseButtonCallback(m_NativeWindow, [](GLFWwindow* window, int button, int action, int mods)
			{
				AD_InputUpdateInfo mouseData;
				mouseData.deviceClass = 0x0000; // "Pointer"
				mouseData.layoutClass = 0x4D53; // "GLFW Mouse"
				mouseData.control = std::to_string(button);
				mouseData.newState = &action;

				InputLayerEvent inputEvent;
				inputEvent.targetLayer = "ICL_Input";
				inputEvent.action = InputLayerAction::Update;
				inputEvent.layerData = &mouseData;

				EventManagerCSR::Notify<ILayerEvent>(&inputEvent);
			}
		);

		// --------------------------------------------------------------------------
		// GLFW Keyboard Event Callbacks --------------------------------------------
		// --------------------------------------------------------------------------

		glfwSetKeyCallback(m_NativeWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				// Not sure how to deal with repeat keys just yet.
				if (action == GLFW_REPEAT)
					return;

				AD_InputUpdateInfo keyData;
				keyData.deviceClass = 0x0001; // "Keyboard"
				keyData.layoutClass = 0x4B42; // "GLFW Keyboard"
				keyData.control = std::to_string(key);
				keyData.newState = &action;

				InputLayerEvent inputEvent;
				inputEvent.targetLayer = "ICL_Input";
				inputEvent.action = InputLayerAction::Update;
				inputEvent.layerData = &keyData;

				EventManagerCSR::Notify<ILayerEvent>(&inputEvent);
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
	}

	void* WindowsWindow::NativeWindow() const
	{
		return m_NativeWindow;
	}

	const WindowProps& WindowsWindow::Properties() const
	{
		return m_Props;
	}

	void WindowsWindow::SetHandle(IWindowHandle* handle)
	{
		glfwSetWindowUserPointer(m_NativeWindow, handle);
	}
}