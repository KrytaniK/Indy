#include "LogMacros.h"
#include <iostream>
#include <memory>

import Indy_Core;
import Indy_Core_Events;

namespace Indy
{
	Application::Application()
	{
		m_LayerStack = std::make_unique<LayerStack>();
	}

	Application::~Application()
	{
		
	}

	void Application::StartAndRun()
	{
		// Application is meant to execute once
		if (m_ShouldClose)
		{
			Run();
			return;
		}

		// Application is meant to run contininuously
		while (!m_ShouldClose)
		{
			m_LayerStack->Update();
			Run();
		}
	}
}