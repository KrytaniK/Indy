#include "Application.h"

#include "Log.h"

#include "Engine/EventSystem/Events.h"

#ifdef ENGINE_PLATFORM_WINDOWS
	#include "Engine/Platform/Windows/WindowsEvents.h"
#endif

namespace Engine
{
	Application::Application()
	{
		// Initialize Core and Client Loggers
		Log::Init();

		// Bind Application "Layer" events
		Events::Bind<Application>("LayerContext", "LayerEvent", this, &Application::onEvent);
		Events::Bind<Application>("LayerContext", "AppClose", this, &Application::onApplicationTerminate);

		// Initialize Application Layers
		m_LayerStack.emplace_back(new WindowLayer()); // Move to std::unique_ptr. Manually creating objects is dangerous.
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