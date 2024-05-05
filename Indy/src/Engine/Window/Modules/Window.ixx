module;

#include <memory>
#include <string>

#include <GLFW/glfw3.h>

export module Indy_Core_Window;

export import :WindowManager;

export
{
	namespace Indy
	{
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
			uint8_t id;
			bool minimized = false, focused = false, vSyncEnabled = true;
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
			friend class WindowManager;

		public:
			virtual ~IWindow() = default;

			virtual void Update() = 0;

			virtual void* NativeWindow() const = 0;
			virtual const WindowProps& Properties() const = 0;

		private:
			virtual void SetHandlePointer(IWindowHandle* handle) = 0;
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

		private:
			virtual void SetHandlePointer(IWindowHandle* handle) override;

		private:
			WindowProps m_Props;
			GLFWwindow* m_NativeWindow;
		};
	}
}