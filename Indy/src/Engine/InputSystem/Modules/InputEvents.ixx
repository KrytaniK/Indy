module;

#include <climits>
#include <string>

export module Indy_Core_InputSystem:Events;

import Indy_Core_EventSystem;

import :Device;

export
{
	namespace Indy
	{
		struct InputDeviceDetectEvent : IEvent
		{
			InputDeviceInfo deviceInfo;
		};

		struct InputDeviceEvent : IEvent
		{
			int deviceFormat = INT_MAX;
			void* deviceState;
			size_t offset;
			size_t size;
			size_t bit = 255;
			int keyToken = INT_MAX;
		};
	}
}