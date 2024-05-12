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

	void Application::PushLayer(const std::shared_ptr<ILayer>& layer)
	{
		m_LayerStack->PushLayer(layer);
	}

	template<typename T>
	void Application::PushLayer()
	{
		static_assert(std::is_base_of<ILayer, T> == true);

		m_LayerStack->PushLayer(std::make_shared<T>());
	}

	void Application::PushOverlay(const std::shared_ptr<ILayer>& overlay)
	{
		m_LayerStack->PushOverlay(overlay);
	}

	template<typename T>
	void Application::PushOverlay()
	{
		static_assert(std::is_base_of<ILayer, T> == true);

		m_LayerStack->PushOverlay(std::make_shared<T>());
	}
}