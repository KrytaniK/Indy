#include "Application.h"

#include "Log.h"

#include "Engine/EventSystem/Events.h"

#include "Engine/Platform/RendererAPI/RenderContext.h"

#include "Engine/LayerStack/WindowLayer/WindowLayer.h"
//#include "Engine/LayerStack/RenderLayer/RenderLayer.h"

#include "Engine/Renderer/Renderer.h"

/* Note:
*	Currently, the Window and Render APIs must be explicity set, or the application crashes. To reduce the ambiguity
*	involved with this approach, it would be in my best interest to default these values, and whenever the API is
*	explicitly changed, the application needs to shutdown the window and the renderer, halt all processes, and
*	re-initialize the window and renderer.
* 
*	Additionally, it might be useful to move the Renderer's functionality into a Render Layer. It's not yet clear
*	whether the CLIENT should have control over the render flow in such an explicit fashion, as missing any step
*	will result in a crash. Encapsulating Render data and processing into its own layer would solve this issue.
*/

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
		Events::Bind<Application>("Window", "Close", this, &Application::onApplicationTerminate);

		// Must be set BEFORE window initialization
		RenderContext::Set(RENDER_API_VULKAN);

		LayerStack::Push(new WindowLayer());

		Renderer::Init();
	}

	Application::~Application()
	{
		LayerStack::Cleanup();
	}

	void Application::onApplicationTerminate(Event& event)
	{
		m_ShouldTerminate = true;
	}
}