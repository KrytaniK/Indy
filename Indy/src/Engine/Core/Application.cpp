#include "Application.h"

#include "Log.h"

#include "Engine/EventSystem/Events.h"

#include "Engine/Platform/WindowAPI/WindowAPI.h"
#include "Engine/Platform/RendererAPI/RendererAPI.h"

#include "Engine/Layers/WindowLayer/WindowLayer.h"
#include "Engine/Layers/RenderLayer/RenderLayer.h"

// To Do:
/*
	- Move layer stack into its own class. There is some functionality that
		needs to be included that std::vector does not support on its own.
*/

namespace Engine
{
	Application::Application()
	{
		// Initialize Core and Client Loggers
		Log::Init();

		// Bind Application "Layer" events
		Events::Bind<Application>("LayerContext", "LayerEvent", this, &Application::onEvent);
		Events::Bind<Application>("LayerContext", "AppClose", this, &Application::onApplicationTerminate);

		// Define Window API & Rendering API (MUST be in this order!)
		WindowAPI::Set(WINDOW_API_GLFW);
		RendererAPI::Set(RENDERER_API_VULKAN);

		// Initialize Application Layers
		m_LayerStack.emplace_back(new WindowLayer());
		m_LayerStack.emplace_back(new RenderLayer());
	}

	Application::~Application()
	{
		// Layer Cleanup
		for (auto it = m_LayerStack.begin(); it != m_LayerStack.end(); ++it)
		{
			delete *it;
		}
	}

	void Application::onEvent(Event& event)
	{
		// Cast event data to needed type
		// handle event data if cast succeeds
		// stop event propagation if needed (This method should technically be the "last" stop for an event)
	}

	void Application::Run()
	{
		// Create application update event
		//	*update events should not hold data.
		Event updateEvent{"LayerContext","AppUpdate"};

		while (!m_ShouldTerminate)
		{
			// Dispatch the update event every frame
			Events::Dispatch(updateEvent);
		}
	}

	void Application::onApplicationTerminate(Event& event)
	{
		// Handle event data if needed

		// Set termination flag
		m_ShouldTerminate = true;
	}
}