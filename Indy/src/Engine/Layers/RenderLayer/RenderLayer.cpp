#include "RenderLayer.h"

namespace Engine
{
	RenderLayer::RenderLayer()
	{
		this->onAttach();
	}

	RenderLayer::~RenderLayer()
	{
		this->onDetach();
	}

	void RenderLayer::onAttach()
	{
		// Bind Event Handles
		Events::Bind<RenderLayer>("LayerContext", "LayerEvent", this, &RenderLayer::onEvent);
		Events::Bind<RenderLayer>("LayerContext", "AppRender", this, &RenderLayer::onRender);

		Renderer::Init();
	}

	void RenderLayer::onDetach()
	{
		Renderer::Shutdown();
	}

	void RenderLayer::onEvent(Event& event)
	{
		
	}

	void RenderLayer::onUpdate(Event& event)
	{
		// Pre-Rendering Setup?
	}

	void RenderLayer::onRender(Event& event)
	{
		Renderer::DrawFrame();
	}
}