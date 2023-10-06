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
		if (!event.IsTerminal())
			return;

		this->TerminateApp();
	}

	void Application::Run()
	{
		// Create application update event
		Events::Event updateEvent;
		updateEvent.Bubbles(false);

		while (m_IsRunning)
		{
			Events::Dispatch<LayerEventContext>(updateEvent);
		}
	}

	void Application::TerminateApp()
	{
		m_IsRunning = false;
	}
}

