#include "WindowsWindow.h"

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

	void* WindowsWindow::GetWindow()
	{
		return m_WindowAPI->GetWindow();
	}
}