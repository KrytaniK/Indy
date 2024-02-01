#include "LogMacros.h"
#include <iostream>

import Indy_Core;
import Indy_Core_EventSystem;
import Indy_Core_LayerStack;

namespace Indy
{
	Application::Application()
	{
		EventManager::AddEventListener<Application, ApplicationEvent>(this, &Application::onEvent);

		LayerStack::Push(new WindowLayer()); // Ensures Window operations are possible
	}

	Application::~Application()
	{
		LayerStack::Cleanup();
	}

	// Base Implementation
	void Application::Run()
	{
		INDY_CORE_INFO("Uh oh! You need to override Application::Run()! Press Enter or Esc to exit.");

		std::cin.get();

		INDY_CORE_INFO("Goodbye!");
	} 

	void Application::Terminate()
	{
		m_Terminate = true;
	}
	
	void Application::onEvent(ApplicationEvent* event)
	{
		if (event->terminate)
			Terminate();
	}

}