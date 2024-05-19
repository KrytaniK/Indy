#include <memory>

#include "LogMacros.h"

import Indy.Application;
import Indy.Layers;

namespace Indy
{
	const Application& Application::Get()
	{
		return *Application::s_Instance;
	}

	Application::Application(const ApplicationCreateInfo& createInfo)
	{
		m_Info.name = createInfo.name;

		m_LayerStack = std::make_unique<LayerStack>();

		m_DeviceManager = std::make_unique<InputDeviceManager>();
		m_WindowManager = std::make_unique<WindowManager>();
	}

	Application::~Application()
	{
		
	}

	void Application::StartAndRun()
	{
		OnLoad();

		if (m_ShouldClose) // Application is meant to execute once
		{
			OnUpdate();
		}
		else // Application is meant to run continuously
		{
			while (!m_ShouldClose)
				OnUpdate();
		}

		OnUnload();
	}
}