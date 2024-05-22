#include <cstdint>

import Indy.Window;
import Indy.Events;

namespace Indy::Window
{
	IWindow* Create(const WindowCreateInfo& createInfo)
	{
		WindowCreateInfo non_const_create_info(createInfo); // can't reference a const reference with a non-const pointer

		WindowCreateEvent event;
		event.createInfo = &non_const_create_info;

		Events<WindowCreateEvent>::Notify(&event);

		return event.outWindow;
	}

	void Destroy(const uint8_t& windowID)
	{
		WindowDestroyEvent event;
		event.windowID = windowID;

		Events<WindowDestroyEvent>::Notify(&event);
	}
}