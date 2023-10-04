#pragma once

#include "Core.h"
#include "Window.h"
#include "Engine/Layers/Layer.h"

#include "Engine/Layers/WindowLayer.h"

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
		virtual void TerminateApp();

	private:
		std::vector<Layer*> m_LayerStack;
		WindowLayer* m_WindowLayer;

	protected:
		bool m_IsRunning = true;
	};

	// Defined in client.
	Application* CreateApplication();
}