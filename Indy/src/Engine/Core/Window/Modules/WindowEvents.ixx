module;

#include <cstdint>

export module Indy.Window:Events;

import :Window;

import Indy.Events;

export
{
	namespace Indy
	{
		struct WindowCreateEvent : IEvent
		{
			WindowCreateInfo* createInfo;
			Window* outWindow;
		};

		struct WindowDestroyEvent : IEvent
		{
			uint8_t windowID;
		};

		struct WindowGetEvent : IEvent
		{
			uint8_t windowID;
			bool getActiveWindow;
			Window* outWindow;
		};
	}
}