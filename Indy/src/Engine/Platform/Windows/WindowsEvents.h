#pragma once

#include "Engine/EventSystem/Events.h"

#include <GLFW/glfw3.h>

namespace Engine
{

	enum WindowEvents {
		Close = 0, Resize, Focus, LoseFocus, Move
	};

	struct WindowCloseEvent
	{
		GLFWwindow* window;
	};

	struct WindowResizeEvent
	{

	};
	
	struct WindowFocusEvent
	{

	};

	struct WindowLoseFocusEvent
	{

	};

	struct WindowMoveEvent
	{

	};
}