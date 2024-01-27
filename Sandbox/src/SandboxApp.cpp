#include <Engine.h>
#include <Engine/Core/ProfileMacros.h>

import Sandbox;
import Indy_Core;
import Time;

namespace Indy
{
	std::unique_ptr<Application> CreateApplication()
	{
		return std::make_unique<Sandbox>();
	};

	Sandbox::Sandbox()
	{
		this->m_Terminate = false;
		this->m_Minimized = false;

		Engine::Events::Bind("Window", "Close", [=](Engine::Event& event) { this->Terminate(); });
		Engine::Events::Bind<Sandbox>("SandboxContext", "AppUpdate", this, &Sandbox::onAppUpdate);
	}

	Sandbox::~Sandbox()
	{
		
	}

	void Sandbox::Run()
	{

		Engine::Event updateEvent{ "SandboxContext","AppUpdate" };

		Engine::Camera camera;
		camera.transform.Translate(0.0f, -2.0f, 2.0f, Engine::Space::Local);
		camera.transform.Rotate(-45.0f, 0.0f, 0.0f, Engine::Space::Local);

		while (!m_Terminate)
		{
			Engine::Events::Dispatch(updateEvent);
			Engine::LayerStack::Update();
			Engine::Renderer::BeginFrame();
			Engine::Renderer::EndFrame(camera);
			Engine::Renderer::DrawFrame();
		}
	}

	void Sandbox::onAppUpdate(Engine::Event& event)
	{
		////////////////////////////////
		// Draw Testing ////////////////
		////////////////////////////////

		std::vector<Engine::Vertex> indexedVerts{
			{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}
		};

		std::vector<uint32_t> indices = {
			0, 1, 2, 2, 3, 0
		};
		Engine::Renderer::DrawIndexed(indexedVerts.data(), static_cast<uint32_t>(indexedVerts.size()), indices.data(), static_cast<uint32_t>(indices.size()));
	}
}