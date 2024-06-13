#include "LogMacros.h"
#include <memory>


import Indy.Application;
import Indy.Layers;
import Indy.Input;
import Indy.Window;
import Indy.Graphics;

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

		OnLoad.Subscribe<Application>(this, &Application::Load);
		OnStart.Subscribe<Application>(this, &Application::Start);
		OnUpdate.Subscribe<Application>(this, &Application::Update);
		OnUnload.Subscribe<Application>(this, &Application::Unload);
	}

	Application::~Application()
	{
		
	}

	void Application::StartAndRun()
	{
		OnLoad.Notify();
		OnStart.Notify();

		if (m_ShouldClose) // Application is meant to execute once
			OnUpdate.Notify();
		else // Application is meant to run continuously
		{
			while (!m_ShouldClose)
				OnUpdate.Notify();
		}

		OnUnload.Notify();
	}
}