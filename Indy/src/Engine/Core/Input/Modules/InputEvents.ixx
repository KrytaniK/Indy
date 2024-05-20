module;

#include <string>
#include <optional>

export module Indy.Input:Events;

import :Device;
import :Control;

import Indy.Events;

export
{
	namespace Indy::Input
	{
		struct Event : IEvent
		{
			std::optional<uint32_t> device_id;
			std::string device_name;
			std::optional<uint32_t> control_id;
			std::string control_alias;
			void* data;
		};
	}
}