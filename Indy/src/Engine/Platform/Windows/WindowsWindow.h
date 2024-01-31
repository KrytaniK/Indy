#pragma once

#include "Engine/Core/Window.h"
//#include "Engine/EventSystem/Events.h"
#include "Engine/Platform/WindowAPI/WindowAPI.h"

namespace Engine
{
	class WindowsWindow : public Window
	{
		public:
			WindowsWindow(const WindowSpec& spec);
			virtual ~WindowsWindow() {};

			void onUpdate() override;
			void* GetWindow() override;

			unsigned int GetWidth() const override { return m_WindowSpec.width; };
			unsigned int GetHeight() const override { return m_WindowSpec.height; };

		private:
			std::unique_ptr<WindowAPI> m_WindowAPI;
			WindowSpec m_WindowSpec;
	};
}