module;

#include <cstdint>

export module Indy_Core_Events:EventTypes;

import Indy_Core_Window;

export
{
	namespace Indy
	{
		struct IEvent
		{
			bool bubbles = true;
			bool propagates = true;
		};

		struct ApplicationEvent : IEvent
		{
			bool terminate = false;
		};

		struct WindowCreateEvent : IEvent
		{
			bool bubbles = false;
			WindowCreateInfo createInfo;
		};

		struct WindowDestroyEvent : IEvent
		{
			uint8_t id = 0;
		};

		struct WindowRequestEvent : IEvent
		{
			bool bubbles = false;
			IWindow* window = nullptr;
		};
	}
}