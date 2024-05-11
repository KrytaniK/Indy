module;

#include <memory>
#include <string>

#include <GLFW/glfw3.h>

export module Indy_Core_Window;

export import :WindowManager;

import Indy_Core_Events;

export
{
	namespace Indy
	{
		class IRenderer;

		struct WindowCreateInfo
		{
			std::string title = "Indy Engine";
			unsigned int width = 1280, height = 760;
			uint8_t id = 0;
		};

		struct WindowProps
		{
			std::string title;
			unsigned int width, height;
			uint8_t id = 0xFF;
			bool minimized = false, focused = false;
		};

		// Base Window Handle
		struct IWindowHandle
		{
			std::weak_ptr<IWindow> window;
			uint8_t index = 0xFF;
		};

		// Abstract interface for platform-specific window implementation
		class IWindow
		{
		public:
			virtual ~IWindow() = default;
			
			virtual void Update() = 0;

			virtual void* NativeWindow() const = 0;
			virtual const WindowProps& Properties() const = 0;

			EventHandler onUpdate;
		};

		// Windows OS Window Implementation
		class WindowsWindow : public IWindow
		{
		public:
			WindowsWindow(const WindowCreateInfo& createInfo);
			virtual ~WindowsWindow();

			virtual void Update() override;

			virtual void* NativeWindow() const override;
			virtual const WindowProps& Properties() const override;

			virtual void SetExtent(const int& width, const int& height) { m_Props.width = width; m_Props.height = height; };
			virtual void SetFocus(bool isFocused) { m_Props.focused = isFocused; };
			virtual void SetMinimized(bool isMinimized) { m_Props.minimized = isMinimized; };

		private:
			WindowProps m_Props;
			GLFWwindow* m_NativeWindow;
		};
	}
}