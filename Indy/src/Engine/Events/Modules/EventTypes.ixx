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
	}
}