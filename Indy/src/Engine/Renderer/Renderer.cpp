#include "Renderer.h"

namespace Engine
{
	std::unique_ptr<RenderContext> Renderer::s_RenderContext = nullptr;

	void Renderer::Init()
	{
		RenderContext::Set(RENDER_API_VULKAN);

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

	void Renderer::EndFrame(Camera& camera)
	{
		s_RenderContext->EndFrame(camera);
	}

	void Renderer::DrawFrame()
	{
		s_RenderContext->DrawFrame();
		s_RenderContext->SwapBuffers();
	}

	void Renderer::Draw(Vertex* vertices, uint32_t vertexCount)
	{
		s_RenderContext->Submit(vertices, vertexCount, nullptr, 0, 1);
	}

	void Renderer::DrawIndexed(Vertex* vertices, uint32_t vertexCount, uint32_t* indices, uint32_t indexCount)
	{
		s_RenderContext->Submit(vertices, vertexCount, indices, indexCount, 1);
	}

	void Renderer::DrawInstanced(Vertex* vertices, uint32_t vertexCount, uint32_t* indices, uint32_t indexCount, uint32_t instanceCount)
	{
		s_RenderContext->Submit(vertices, vertexCount, indices, indexCount, instanceCount);
	}

}