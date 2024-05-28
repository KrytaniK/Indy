module;

#include <string>
#include <cstdint>

export module Indy.Window:Window;

import Indy.Input;

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

		// Abstract interface for platform-specific window implementation
		class Window
		{
		public:
			static Window* Create(const WindowCreateInfo& createInfo);
			static void Destroy(const uint8_t& windowID);

		public:
			virtual ~Window() = default;

			virtual void Update() = 0;

			virtual void* NativeWindow() const = 0;
			virtual const WindowProps& Properties() const = 0;

			virtual Input::Context* GetInputContext() = 0;
		};
	}
}