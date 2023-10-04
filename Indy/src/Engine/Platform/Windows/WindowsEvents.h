#pragma once

#include "Engine/EventSystem/Events.h"

struct GLFWwindow;

namespace Engine
{
	// Window Events
	struct GLFW_Event : Events::Event 
	{
		GLFWwindow* window;
	};
	
	struct WindowCloseEvent : GLFW_Event
	{
		bool b_AppShouldTerminate;
	};

	struct WindowResizeEvent : GLFW_Event
	{
		int width, height;
	};
	
	struct WindowFocusEvent : GLFW_Event {};

	struct WindowLoseFocusEvent : GLFW_Event {};

	struct WindowMoveEvent : GLFW_Event
	{
		int xpos, ypos;
	};

	// General Input Events
	struct ScrollEvent : GLFW_Event
	{
		double xoffset, yoffset;
	};

	// Mouse Events

	struct MouseMoveEvent : GLFW_Event
	{
		double xpos, ypos;
	};

	struct MouseButtonEvent : GLFW_Event
	{
		int button, action, mods;
	};

	// Keyboard Events
	struct KeyboardEvent : GLFW_Event
	{
		int key, scancode, action, mods;
	};
}