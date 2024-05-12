module;

#include <string>
#include <memory>

export module Indy.Window:IWindow;

import Indy.Events;

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
			uint8_t id = 0xFF;
			bool minimized = false, focused = false;
		};

		class IWindow;

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
	}
}