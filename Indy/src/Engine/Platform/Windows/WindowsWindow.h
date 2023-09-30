#pragma once

#include "Engine/Core/Window.h"
#include "Engine/EventSystem/Events.h"
#include "WindowsEvents.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	class WindowsWindow : public Window
	{
		private:
			// Event Handles
			void onWindowClose(const WindowCloseEvent& event);
			void onWindowResize(const WindowResizeEvent& event);
			void onWindowMove(const WindowMoveEvent& event);
			void onWindowFocus(const WindowFocusEvent& event);
			void onWindowLoseFocus(const WindowLoseFocusEvent& event);

		public:
			WindowsWindow(const WindowSpec& spec);
			virtual ~WindowsWindow();

			void onUpdate() override;

			unsigned int GetWidth() const override { return m_WindowSpec.width; };
			unsigned int GetHeight() const override { return m_WindowSpec.height; };

		private:
			void BindApplicationEvents();
			std::vector<EventHandle> m_eventHandles;

			GLFWwindow* m_GLFW_Window;

			WindowSpec m_WindowSpec;
	};
}