module;

#include <string>
#include <memory>
#include <functional>

export module Indy.Window:IWindow;

import Indy.Renderer;
import Indy.Input;

export
{
	namespace Indy
	{
		struct WindowCreateInfo
		{
			std::string title = "Indy Engine";
			unsigned int width = 1280, height = 760;
			RenderAPI renderApi = RenderAPI::None;
			uint8_t id = 0;
		};

		struct WindowProps
		{
			std::string title;
			unsigned int width, height;
			uint8_t id = 0xFF;
			RenderAPI renderApi = RenderAPI::None;
			bool minimized = false, focused = false;
		};

		// Abstract interface for platform-specific window implementation
		class IWindow
		{
		public:
			virtual ~IWindow() = default;

			virtual void Update() = 0;

			virtual void* NativeWindow() const = 0;
			virtual const WindowProps& Properties() const = 0;

		protected:
			std::shared_ptr<InputContext> m_InputContext;
			std::unique_ptr<IRenderer> m_Renderer;
		};
	}
}