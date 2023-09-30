#pragma once

#include "Engine/EventSystem/Events.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	// Window Events

	struct WindowCloseEvent
	{
		GLFWwindow* window;
		bool b_AppShouldTerminate;
	};

	struct WindowResizeEvent
	{
		GLFWwindow* window;
		int width, height;
	};
	
	struct WindowFocusEvent
	{
		GLFWwindow* window;
	};

	struct WindowLoseFocusEvent
	{
		GLFWwindow* window;
	};

	struct WindowMoveEvent
	{
		GLFWwindow* window;
		int xpos, ypos;
	};

	// General Input Events
	struct ScrollEvent
	{
		GLFWwindow* window;
		double xoffset, yoffset;
	};

	// Mouse Events

	struct MouseMoveEvent
	{
		GLFWwindow* window;
		double xpos, ypos;
	};

	struct MouseButtonEvent
	{
		GLFWwindow* window;
		int button, action, mods;
	};

	// Keyboard Events
	struct KeyboardEvent
	{
		GLFWwindow* window;
		int key, scancode, action, mods;
	};
}