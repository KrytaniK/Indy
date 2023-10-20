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
		Events::Bind<RenderLayer>("LayerContext", "AppUpdate", this, &RenderLayer::onUpdate);

		// Create the renderer instance
		m_Renderer = std::make_unique<Renderer>();

		INDY_CORE_TRACE("Attached Render Layer");
	}

	void RenderLayer::onDetach()
	{

	}

	void RenderLayer::onEvent(Event& event)
	{
		
	}

	void RenderLayer::onUpdate(Event& event)
	{

	}
}