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
		Events::Bind<WindowLayer>("LayerContext", "RequestWindow", this, &WindowLayer::onRequestWindow);

		// Create the window instance
		m_Window = std::unique_ptr<Window>(Window::Create());
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

	void WindowLayer::onRequestWindow(Event& event)
	{
		event.data = m_Window->GetWindow();
	}
}
