#include "Application.h"

#include "Log.h"

#include "Engine/EventSystem/Events.h";

#ifdef ENGINE_PLATFORM_WINDOWS
	#include "Engine/Platform/Windows/WindowsEvents.h"
#endif

namespace Engine
{
	Application::Application()
	{
		Log::Init();

		m_Window = std::unique_ptr<Window>(Window::Create());
		
		// Terminate our application if the window closes
		Events::Bind<WindowCloseEvent>([this](const WindowCloseEvent& event) 
			{ if (event.b_AppShouldTerminate) TerminateApp(); });
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		while (m_IsRunning)
		{
			m_Window->onUpdate();
		}
	}

	void Application::TerminateApp()
	{
		m_IsRunning = false;
	}
}

