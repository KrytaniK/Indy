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

				#pragma region Draw Testing

				// The best part is that it doesn't matter where we call Renderer::Draw() or Renderer::DrawIndexed()
				//	so long as it happens before Renderer::EndFrame(). This is because draw calls are processed and submitted
				//	to a "queue" in the command pool that gets emptied and cleared after the recording phase. 

				/* I still need to implement per-object rendering, or more accurately, "Meshes". Right now, only one model matrix
				*	is allowed during rendering, tied to the view matrix. Later, a Camera object will directly influence the view
				*	matrix, and the the model matrix will be seperate, so that meshes can be rendered individually at different
				*	locations, and the user can move the camera through input.
				*/

				std::vector<Vertex> drawVerts{
					{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
					{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
					{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
					{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
					{{-0.5f, 0.5f, 0.0f}, {0.5f, 0.5f, 0.0f}},
					{ {-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}
				};

				std::vector<Vertex> indexedVerts{
					{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
					{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
					{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
					{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}
				};

				std::vector<uint32_t> indices = {
					0, 1, 2, 2, 3, 0
				};

				//Renderer::Draw(drawVerts.data(), drawVerts.size());
				Renderer::DrawIndexed(indexedVerts.data(), indexedVerts.size(), indices.data(), indices.size());

				#pragma endregion

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