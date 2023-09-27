#pragma once

#include "Core.h"
#include "Window.h"

namespace Engine
{
	class ENGINE_API Application
	{
	public:
		Application();
		virtual ~Application();

		virtual void Run();

	protected:
		std::unique_ptr<Window> m_Window;

		bool m_IsRunning = true;
	};

	// Defined in client.
	Application* CreateApplication();
}
