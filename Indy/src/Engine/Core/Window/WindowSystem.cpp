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

		// Sync Application Events
		Application& app = Application::Get();
		app.Load.Subscribe<WindowSystem>(this, &WindowSystem::OnLoad);
		app.Update.Subscribe<WindowSystem>(this, &WindowSystem::OnUpdate);
		app.Unload.Subscribe<WindowSystem>(this, &WindowSystem::OnUnload);

		// Bind Event Handles
		Events<WindowCreateEvent>::Subscribe<WindowSystem>(this, &WindowSystem::OnWindowCreate);
		Events<WindowDestroyEvent>::Subscribe<WindowSystem>(this, &WindowSystem::OnWindowDestroy);
		Events<WindowGetEvent>::Subscribe<WindowSystem>(this, &WindowSystem::OnWindowGet);
	}

	WindowSystem::~WindowSystem()
	{

	}

	void WindowSystem::OnLoad()
	{
		// Initialize Window API
		glfwInit();

		glfwSetErrorCallback([](int error, const char* description)
			{
				INDY_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
			}
		);

		// Initialize Graphics API
		m_GraphicsAPI = GraphicsAPI::Create(GraphicsAPI::Vulkan);
	}

	void WindowSystem::OnUpdate()
	{
		m_WindowManager->Update();
	}

	void WindowSystem::OnUnload()
	{
		
	}

	void WindowSystem::OnWindowCreate(WindowCreateEvent* event)
	{
		event->outWindow = m_WindowManager->AddWindow(*event->createInfo); // Create the window

		// Bail early if window handle or graphics API is invalid
		if (!event->outWindow || !m_GraphicsAPI)
			return;

		m_GraphicsAPI->CreateRenderTarget(event->outWindow); // Generate a render target
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
