#pragma once

#include "Core.h"
#include "Window.h"
#include "Engine/Layers/Layer.h"

#include <vector>

namespace Engine
{
	class ENGINE_API Application
	{
	public:
		Application();

	public:
		virtual ~Application();

		virtual void Run();
		virtual void onEvent(Event& event);
		virtual void onApplicationTerminate(Event& event);

	private:
		std::vector<Layer*> m_LayerStack;

	protected:
		bool m_ShouldTerminate = false;
	};

	// Defined in client.
	Application* CreateApplication();
}