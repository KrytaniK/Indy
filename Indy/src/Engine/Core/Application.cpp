#include <memory>

import Indy.Application;
import Indy.Layers;

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
		Start();

		// Application is meant to execute once
		if (m_ShouldClose)
		{
			Run();
			return;
		}

		// Application is meant to run continuously
		while (!m_ShouldClose)
		{
			m_LayerStack->Update();
			Run();
		}
	}

	void Application::PushLayer(const std::shared_ptr<ILayer>& layer)
	{
		m_LayerStack->PushLayer(layer);
	}

	void Application::PushOverlay(const std::shared_ptr<ILayer>& overlay)
	{
		m_LayerStack->PushOverlay(overlay);
	}
}