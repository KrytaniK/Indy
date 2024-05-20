#include <memory>

import Indy.Application;
import Indy.Window;
import Indy.Events;

namespace Indy
{
	WindowSystem::WindowSystem()
	{
		m_WindowManager = std::make_unique<WindowManager>();

		// Sync Application Events
		Application& app = Application::Get();
		app.OnLoad_Event.Subscribe<WindowSystem>(this, &WindowSystem::OnLoad);
		app.OnUpdate_Event.Subscribe<WindowSystem>(this, &WindowSystem::OnUpdate);
		app.OnUnload_Event.Subscribe<WindowSystem>(this, &WindowSystem::OnUnload);

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
		// Initialize Window API (GLFW, SDL)

		// Initialize Render API (Vulkan, DirectX, Metal, OpenGL)
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
		m_WindowManager->AddWindow(*event->createInfo);

		event->outWindow = m_WindowManager->GetWindow(event->createInfo->id);
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
