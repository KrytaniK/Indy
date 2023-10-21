#include "Renderer.h"

namespace Engine
{
	Renderer::Renderer()
	{
		// Create and initialize the rendering API.
		m_RendererAPI = std::unique_ptr<RendererAPI>(RendererAPI::Create());
		m_RendererAPI->Init();
	}

	Renderer::~Renderer()
	{
		m_RendererAPI->Shutdown();
	}
}