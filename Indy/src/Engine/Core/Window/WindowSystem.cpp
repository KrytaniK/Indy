#include <Engine/Core/LogMacros.h>
#include <memory>

#include <GLFW/glfw3.h>

import Indy.Application;
import Indy.Graphics;
import Indy.Window;
import Indy.Events;

namespace Indy
{
	WindowSystem::WindowSystem()
	{
		m_WindowManager = std::make_unique<WindowManager>();

		// Bind Event Handles
		Events<WindowCreateEvent>::Subscribe<WindowSystem>(this, &WindowSystem::OnWindowCreate);
		Events<WindowDestroyEvent>::Subscribe<WindowSystem>(this, &WindowSystem::OnWindowDestroy);
		Events<WindowGetEvent>::Subscribe<WindowSystem>(this, &WindowSystem::OnWindowGet);

		// Initialize GLFW for cross-platform window handling
		glfwInit();

		glfwSetErrorCallback([](int error, const char* description)
			{
				INDY_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
			}
		);
	}

	WindowSystem::~WindowSystem()
	{

	}

	void WindowSystem::OnUpdate()
	{
		m_WindowManager->Update();
	}

	void WindowSystem::OnWindowCreate(WindowCreateEvent* event)
	{
		m_WindowManager->AddWindow(*event->createInfo);
	}

	void WindowSystem::OnWindowDestroy(WindowDestroyEvent* event)
	{
		m_WindowManager->DestroyWindow(event->windowID);
	}

	void WindowSystem::OnWindowGet(WindowGetEvent* event)
	{
		if (event->getActiveWindow)
			event->outWindow = m_WindowManager->GetActiveWindow();
		else
			event->outWindow = m_WindowManager->GetWindow(event->windowID);
	}
}
