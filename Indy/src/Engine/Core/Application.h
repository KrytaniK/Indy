#pragma once

#include "Window.h"
#include "Engine/LayerStack/LayerStack.h"

#include <vector>

namespace Engine
{
	class Application
	{
	public:
		Application();

	public:
		virtual ~Application();

		virtual void Run() = 0; // Run loop defined by CLIENT

		virtual void onApplicationTerminate(Event& event); // <-- Might need to be private

	protected:
		bool m_ShouldTerminate = false;
		bool m_Minimized = false;
	};

	// Defined in client.
	Application* CreateApplication();
}