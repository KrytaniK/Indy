export module EventSystem:EventTypes;

import Indy_Core;

export
{
	namespace Indy
	{
		struct IEvent
		{
			bool bubbles;
			bool propagates;
		};

		struct WindowCreateEvent : IEvent 
		{
			WindowCreateInfo createInfo;
		};
	}
}