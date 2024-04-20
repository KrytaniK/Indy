#include "LogMacros.h"
#include <iostream>
#include <memory>

import Indy_Core;
import Indy_Core_LayerStack;
import Indy_Core_EventSystem;

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
		while (!m_ShouldClose)
		{
			m_LayerStack->Update();
			Run();
		}
	}
}