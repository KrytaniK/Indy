#include "WindowLayer.h"

#include "Engine/Platform/WindowAPI/WindowAPI.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	void WindowLayer::onAttach()
	{
		Events::Bind<WindowLayer>("LayerContext", "RequestWindow", this, &WindowLayer::onRequestWindow);

		WindowAPI::Set(WINDOW_API_GLFW);

		// Create the window instance
		m_Window = std::unique_ptr<Window>(Window::Create());
	}

	void WindowLayer::onDetach() {};

	void WindowLayer::Update()
	{
		m_Window->onUpdate();
	}

	void WindowLayer::onRequestWindow(Event& event)
	{
		event.data = m_Window->GetWindow();
	}
}
