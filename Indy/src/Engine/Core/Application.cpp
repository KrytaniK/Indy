#include "LogMacros.h"
#include <memory>


import Indy.Application;
import Indy.Layers;
import Indy.Window;
import Indy.Graphics;

namespace Indy
{
	Application::Application(const ApplicationCreateInfo& createInfo)
	{
		m_Info.name = createInfo.name;
	}

	Application::~Application()
	{
		
	}

	void Application::StartAndRun()
	{
		// While loop to support application restarting.
		m_Restart = true;
		while (m_Restart)
		{
			m_Restart = false;

			// Load app data
			Load();

			// Start core processes
			Start();

			// Run once, even if m_ShouldClose = false
			Update();

			// Primary run loop (m_ShouldClose = true)
			while (!m_ShouldClose)
				Update();

			// Shutdown core processes
			Shutdown();

			// Unload app data
			Unload();
		}
	}
}