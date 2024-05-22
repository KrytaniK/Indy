#include "LogMacros.h"
#include <memory>


import Indy.Application;
import Indy.Layers;
import Indy.Input;
import Indy.Window;

namespace Indy
{
	Application* Application::s_Instance = nullptr;

	Application& Application::Get()
	{
		return *Application::s_Instance;
	}

	Application::Application(const ApplicationCreateInfo& createInfo)
	{
		m_Info.name = createInfo.name;

		Application::s_Instance = this;

		m_LayerStack = std::make_unique<LayerStack>();
		m_InputSystem = std::make_unique<InputSystem>();
		m_WindowSystem = std::make_unique<WindowSystem>();

		Load.Subscribe<Application>(this, &Application::OnLoad);
		OnStart_Event.Subscribe<Application>(this, &Application::OnStart);
		Update.Subscribe<Application>(this, &Application::OnUpdate);
		Unload.Subscribe<Application>(this, &Application::OnUnload);
	}

	Application::~Application()
	{
		
	}

	void Application::StartAndRun()
	{
		Load.Notify();

		OnStart_Event.Notify();

		if (m_ShouldClose) // Application is meant to execute once
		{
			Update.Notify();
		}
		else // Application is meant to run continuously
		{
			while (!m_ShouldClose)
			{
				Update.Notify();
			}
		}

		Unload.Notify();
	}
}