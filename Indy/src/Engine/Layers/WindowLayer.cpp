#include "WindowLayer.h"

#include "Engine/Platform/Windows/WindowsEvents.h"

#include <GLFW/glfw3.h>

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
		// Bind Event Handles
		Events::Bind<WindowLayer>("LayerContext", "LayerEvent", this, &WindowLayer::onEvent);
		Events::Bind<WindowLayer>("LayerContext", "AppUpdate", this, &WindowLayer::onUpdate);

		// Create the window
		m_Window = std::unique_ptr<Window>(Window::Create());
	}

	void WindowLayer::onDetach()
	{
		
	}

	void WindowLayer::onUpdate(Event& event)
	{
		// Deal with any event data

		// Update GLFWwindow
		m_Window->onUpdate();
	}

	void WindowLayer::onEvent(Event& event)
	{
		// Cast event data to needed type
		// handle event data if cast succeeds
		// stop event propagation if needed
	}
}
