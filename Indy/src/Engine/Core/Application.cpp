#include "Application.h"

#include "Log.h"

#include "Engine/EventSystem/Events.h"
#include "Engine/Layers/LayerEventContext.h"

#ifdef ENGINE_PLATFORM_WINDOWS
	#include "Engine/Platform/Windows/WindowsEvents.h"
#endif

namespace Engine
{
	Application::Application()
	{
		// Initialize Core and Client Loggers
		Log::Init();

		INDY_CORE_TRACE("Binding Application Event Handles...");
		Events::Bind<Application, LayerEventContext>(this, &Application::onEvent);

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

	void Application::onEvent(Events::Event& event)
	{
		if (event.type != Events::EventType::ApplicationShutdown) return;

		/* It's likely that other layers need to shut down before the application terminates.
			Because of this, it would be better to have this event propagate from the top of the
			layer stack, downward to this "layer".
		*/ 
		event.StopPropagation(); // Shutdown event should not propagate (maybe) (shouldn't for now)

		INDY_CORE_WARN("Application Shutting Down");

		this->TerminateApp();
	}

	void Application::Run()
	{
		while (m_IsRunning)
		{
			Events::Event updateEvent;
			updateEvent.type = Events::EventType::ApplicationUpdate;

			Events::Dispatch<LayerEventContext>(updateEvent);
		}
	}

	void Application::TerminateApp()
	{
		m_IsRunning = false;
	}
}

