module;

#include <cstdint>

export module Indy.Window;

export import :IWindow;
export import :Manager;
export import :Events;
export import :System;

export
{
	namespace Indy::Window
	{
		IWindow* Create(const WindowCreateInfo& createInfo);
		void Destroy(const uint8_t& windowID);
	}
}