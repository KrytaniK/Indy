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
	- Implement a Mesh System & sync model matrices in render API.
	- Time Tracking

	- (Document): Event System
	- (Document): Renderer

	- (Think): Should the core application class explicitly handle events & app termination code?
	- (Think): Should the CLIENT have the ability to define the render flow through the use of Renderer::BeginFrame(), etc.?
*/

namespace Engine
{
	Application::Application()
	{
		// Initialize Debug Logging
		Log::Init();

		// Bind Application "Layer" events (Application Class is techinically a layer)
		Events::Bind<Application>("LayerContext", "AppClose", this, &Application::onApplicationTerminate);

		// Define Window & Rendering APIs, respectively (MUST be in this order, and before layer creation!)
		// It would be useful to allow the user to select the Rendering API before startup.
		WindowAPI::Set(WINDOW_API_GLFW);
		RenderContext::Set(RENDERER_API_VULKAN);

		// Initialize Application Layers
		m_LayerStack.emplace_back(new WindowLayer());

		// Initialize Renderer
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

	void Application::onApplicationTerminate(Event& event)
	{
		// Handle event data if needed

		// Set termination flag
		m_ShouldTerminate = true;
	}
}