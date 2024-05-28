#include <cstdint>

import Indy.Window;
import Indy.Events;

namespace Indy
{
	Window* Window::Create(const WindowCreateInfo& createInfo)
	{
		WindowCreateInfo non_const_create_info(createInfo); // can't reference a const reference with a non-const pointer

		// Send out window create event
		WindowCreateEvent event;
		event.createInfo = &non_const_create_info;

		Events<WindowCreateEvent>::Notify(&event);

		return event.outWindow;
	}

	void Window::Destroy(const uint8_t& windowID)
	{
		// Send out window destroy event
		WindowDestroyEvent event;
		event.windowID = windowID;

		Events<WindowDestroyEvent>::Notify(&event);
	}
}