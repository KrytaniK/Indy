#include "Application.h"

#include "Log.h"

#include "Engine/EventSystem/Events.h"

#include "Engine/Platform/WindowAPI/WindowAPI.h"
#include "Engine/Platform/RendererAPI/RenderContext.h"

#include "Engine/Layers/WindowLayer/WindowLayer.h"

#include "Engine/Renderer/Renderer.h"

// To Do:
/*
	- Move layer stack into its own class. There is some functionality that
		needs to be included that std::vector does not support on its own.
*/

namespace Engine
{
	Application::Application()
	{
		// Initialize Debug Logging
		Log::Init();

		// Bind Application "Layer" events (Application Class is techinically a layer)
		Events::Bind<Application>("LayerContext", "LayerEvent", this, &Application::onEvent);
		Events::Bind<Application>("LayerContext", "AppClose", this, &Application::onApplicationTerminate);

		// Define Window & Rendering APIs, respectively (MUST be in this order, and before layer creation!)
		// In the future, it might be nice to allow hot-swapping of these
		WindowAPI::Set(WINDOW_API_GLFW);
		RenderContext::Set(RENDERER_API_VULKAN);

		// Initialize Application Layers (implicitly creates a window)
		m_LayerStack.emplace_back(new WindowLayer());

		Renderer::Init();


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

	/* TODO: Time Tracking
	*	- Tick events determine the frame rate at which the application updates (usually 16ms/60fps)
	*	- Update events process state, physics, and may perform heavier calculations. These should happen less frequently (can be around 64ms/15fps)
	*/
	void Application::Run()
	{
		Event updateEvent{"LayerContext","AppUpdate"};
		Event renderEvent{"RenderContext","AppRender"};

		while (!m_ShouldTerminate)
		{
			if (!m_Minimized)
			{
				Events::Dispatch(updateEvent);
				// Dispatch Tick Event

				// Begin recording render commands, initialize render pass
				Renderer::BeginFrame();

				// Ensure all vertex/index data is submitted and uniform data is updated
				Events::Dispatch(renderEvent);
					
				// Issue the draw call, end render pass and command recording
				Renderer::EndFrame();

				// Present the frame
				Renderer::DrawFrame();
			}
		}

		// Stop Time Tracking
	}

	void Application::onApplicationTerminate(Event& event)
	{
		// Handle event data if needed

		// Set termination flag
		m_ShouldTerminate = true;
	}
}