#include "Renderer.h"

namespace Engine
{
	struct Vec2
	{
		float x, y;
	};

	struct Vec3
	{
		float x, y, z;
	};

	struct Vertex
	{
		Vec2 pos;
		Vec3 color;
	};

	std::unique_ptr<RenderContext> Renderer::s_RenderContext = nullptr;

	void Renderer::Init()
	{
		s_RenderContext = std::unique_ptr<RenderContext>(RenderContext::Create());

		Event event{ "LayerContext","RequestWindow" };
		Events::Dispatch(event);

		s_RenderContext->Init(event.data);
	}

	void Renderer::Shutdown()
	{
		s_RenderContext->Shutdown();
	}

	void Renderer::BeginFrame()
	{
		// Start Recording Command Buffer
		s_RenderContext->BeginFrame();
	}

	void Renderer::EndFrame()
	{
		s_RenderContext->EndFrame();
	}

	void Renderer::DrawFrame()
	{
		s_RenderContext->DrawFrame();
		s_RenderContext->SwapBuffers();
	}
}