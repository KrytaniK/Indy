#include "WindowLayer.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	WindowLayer::WindowLayer()
	{
		this->onAttach();
	}

	WindowLayer::~WindowLayer()
	{
		this->onDetach();
	}

	void WindowLayer::onAttach()
	{
		// Bind Event Handles
		Events::Bind<WindowLayer>("LayerContext", "LayerEvent", this, &WindowLayer::onEvent);
		Events::Bind<WindowLayer>("LayerContext", "AppUpdate", this, &WindowLayer::onUpdate);

		// Create the window instance
		m_Window = std::unique_ptr<Window>(Window::Create());

		INDY_CORE_TRACE("Attached Window Layer");
	}

	void WindowLayer::onDetach()
	{
		
	}

	void WindowLayer::onUpdate(Event& event)
	{
		// Deal with any event data

		m_Window->onUpdate();
	}

	void WindowLayer::onEvent(Event& event)
	{
		
	}
}
