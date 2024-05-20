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

		OnLoad_Event.Subscribe<Application>(this, &Application::OnLoad);
		OnUpdate_Event.Subscribe<Application>(this, &Application::OnUpdate);
		OnUnload_Event.Subscribe<Application>(this, &Application::OnUnload);
	}

	Application::~Application()
	{
		
	}

	void Application::StartAndRun()
	{
		OnLoad_Event.Notify();

		if (m_ShouldClose) // Application is meant to execute once
		{
			OnUpdate_Event.Notify();
		}
		else // Application is meant to run continuously
		{
			while (!m_ShouldClose)
			{
				OnUpdate_Event.Notify();
			}
		}

		OnUnload_Event.Notify();
	}
}