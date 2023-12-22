#include "SandboxApp.h"

/* Sandbox utilizes the core Engine to make "something". The purpose of this vsProject
*	is simply to test the capabilites of the engine and ensure everything works as
*	intended.
*/

namespace Engine
{
	Application* CreateApplication()
	{
		return new Sandbox();
	}

	Sandbox::Sandbox() 
	{
		Events::Bind<Sandbox>("SandboxContext", "AppUpdate", this, &Sandbox::onAppUpdate);

		m_Camera.transform.Translate(0.0f, -2.0f, 2.0f, Space::Local);
		m_Camera.transform.Rotate(-45.0f, 0.0f, 0.0f, Space::Local);
	};

	Sandbox::~Sandbox() {};

	void Sandbox::Run()
	{
		Event updateEvent{"SandboxContext","AppUpdate"};

		while (!m_ShouldTerminate)
		{
			if (!m_Minimized)
			{
				LayerStack::Update();

				Events::Dispatch(updateEvent);

				// Begin recording render commands, initialize render pass
				Renderer::BeginFrame();

				// Issue the draw call, end render pass and command recording
				Renderer::EndFrame(m_Camera);

				// Present the frame
				Renderer::DrawFrame();
			}
		}
	}

	void Sandbox::onAppUpdate(Event& event)
	{
		////////////////////////////////
		// Draw Testing ////////////////
		////////////////////////////////

		std::vector<Vertex> indexedVerts{
			{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}
		};

		std::vector<uint32_t> indices = {
			0, 1, 2, 2, 3, 0
		};

		Renderer::DrawIndexed(indexedVerts.data(), static_cast<uint32_t>(indexedVerts.size()), indices.data(), static_cast<uint32_t>(indices.size()));
	}

}