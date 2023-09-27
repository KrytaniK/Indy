#include "Application.h"

#include "Log.h"

namespace Engine
{
	Application::Application()
	{
		Log::Init();

		m_Window = std::unique_ptr<Window>(Window::Create());
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		INDY_CORE_INFO("App Running!");
		while (m_IsRunning)
		{
			m_Window->onUpdate();
		}
		INDY_CORE_INFO("App Stopping!");
	}
}

