#include <cstdint>

import Indy.Window;
import Indy.Events;

namespace Indy
{
	Window* Window::Create(WindowCreateInfo& createInfo)
	{
		// Send out window create event
		WindowCreateEvent event;
		event.createInfo = &createInfo;

		Events<WindowCreateEvent>::Notify(&event);
		
		// Retrieve the created window
		WindowGetEvent getEvent;
		getEvent.windowID = createInfo.id;

		Events<WindowGetEvent>::Notify(&getEvent);
		return getEvent.outWindow;
	}

	void Window::Destroy(const uint8_t& windowID)
	{
		// Send out window destroy event
		WindowDestroyEvent event;
		event.windowID = windowID;

		Events<WindowDestroyEvent>::Notify(&event);
	}
}