#include "WindowLayer.h"

#include "LayerEventContext.h"

namespace Engine
{
	WindowLayer::WindowLayer()
	{
		// Initialize Relevant Components
		this->onAttach();
	}

	WindowLayer::~WindowLayer()
	{
		this->onDetach();
	}

	void WindowLayer::onAttach()
	{
		// Connect this layer through the event system.
		m_EventHandle = Events::Bind<WindowLayer, LayerEventContext>(this, &WindowLayer::onEvent);

		// Create the window
		m_Window = std::unique_ptr<Window>(Window::Create());
	}

	void WindowLayer::onDetach()
	{
		Events::UnBind<LayerEventContext>(m_EventHandle);
	}

	void WindowLayer::onUpdate()
	{
		m_Window->onUpdate();
	}

	void WindowLayer::onEvent(Events::Event& event)
	{
		if (event.type == Events::EventType::ApplicationUpdate)
		{
			this->onUpdate();
			return;
		}
	}
}