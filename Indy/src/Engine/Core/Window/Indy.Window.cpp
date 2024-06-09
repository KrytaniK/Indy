#include <cstdint>

import Indy.Window;
import Indy.Events;

namespace Indy
{
	void Window::Create(WindowCreateInfo& createInfo)
	{
		// Send out window create event
		WindowCreateEvent event;
		event.createInfo = &createInfo;

		Events<WindowCreateEvent>::Notify(&event);
	}

	void Window::Destroy(const uint8_t& windowID)
	{
		// Send out window destroy event
		WindowDestroyEvent event;
		event.windowID = windowID;

		Events<WindowDestroyEvent>::Notify(&event);
	}
}