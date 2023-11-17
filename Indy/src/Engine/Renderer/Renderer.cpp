#include "Renderer.h"

namespace Engine
{
	std::unique_ptr<RendererAPI> Renderer::s_RendererAPI = nullptr;

	void Renderer::Init()
	{
		s_RendererAPI = std::unique_ptr<RendererAPI>(RendererAPI::Create());
		s_RendererAPI->Init();
	}

	void Renderer::Shutdown()
	{
		s_RendererAPI->Shutdown();
	}

	void Renderer::DrawFrame()
	{
		s_RendererAPI->DrawFrame();
	}
}