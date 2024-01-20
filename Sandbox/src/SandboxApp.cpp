#include "SandboxApp.h"

namespace Indy
{
	using namespace Engine;

	std::unique_ptr<Application> CreateApplication()
	{
		return std::make_unique<Sandbox>();
	};

	Sandbox::Sandbox()
	{
		this->m_Terminate = false;
		this->m_Minimized = false;
		std::cout << "Sandbox Constructor" << std::endl;

		Events::Bind("Window", "Close", [=](Event& event) { this->Terminate(); });
		Events::Bind<Sandbox>("SandboxContext", "AppUpdate", this, &Sandbox::onAppUpdate);
	}

	Sandbox::~Sandbox()
	{
		std::cout << "Sandbox Destructor" << std::endl;
	}

	void Sandbox::Run()
	{
		Event updateEvent{ "SandboxContext","AppUpdate" };

		Camera camera;
		camera.transform.Translate(0.0f, -2.0f, 2.0f, Space::Local);
		camera.transform.Rotate(-45.0f, 0.0f, 0.0f, Space::Local);

		while (!m_Terminate)
		{
			if (!m_Minimized)
			{
				Events::Dispatch(updateEvent);

				LayerStack::Update();

				Renderer::BeginFrame();

				Renderer::EndFrame(camera);

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