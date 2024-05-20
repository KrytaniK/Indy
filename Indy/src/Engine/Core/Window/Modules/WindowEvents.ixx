module;

#include <cstdint>

export module Indy.Window:Events;

import :IWindow;

import Indy.Events;

export
{
	namespace Indy
	{
		struct WindowCreateEvent : IEvent
		{
			WindowCreateInfo* createInfo;
			IWindow* outWindow;
		};

		struct WindowDestroyEvent : IEvent
		{
			uint8_t windowID;
		};

		struct WindowGetEvent : IEvent
		{
			uint8_t windowID;
			bool getActiveWindow;
			IWindow* outWindow;
		};
	}
}