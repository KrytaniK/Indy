#include "WindowAPI.h"

#include "Engine/Core/LogMacros.h"
#include "Engine/EventSystem/Events.h"
#include "GLFW/GLFWWindowAPI.h"

namespace Engine
{
	WindowAPI::Type WindowAPI::s_Type = WindowAPI::Type::None;

	// Creates an instance of the Window API set by the application. Defaults to GLFW.
	std::unique_ptr<WindowAPI> WindowAPI::Create(const WindowSpec& windowSpec)
	{
		switch (s_Type)
		{
		case Type::GLFW:
			return std::make_unique<GLFW_WindowAPI>(windowSpec);
		default:
			return std::make_unique<GLFW_WindowAPI>(windowSpec);
		}
	}
}