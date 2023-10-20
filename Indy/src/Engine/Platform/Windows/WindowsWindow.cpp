#include "WindowsWindow.h"

#include "Engine/Platform/RendererAPI/RendererAPI.h"

namespace Engine
{
	WindowsWindow::WindowsWindow(const WindowSpec& spec)
	{
		m_WindowSpec = spec;

		// Initialize the window backend (GLFW in this case); Creates a window internally.
		m_WindowAPI = std::unique_ptr<WindowAPI>(WindowAPI::Create(spec));
	}

	void WindowsWindow::onUpdate()
	{
		m_WindowAPI->onUpdate();
	}
}