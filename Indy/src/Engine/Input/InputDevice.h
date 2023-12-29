#pragma once

#include "InputTypes.h"
#include "InputDescription.h"

#include <map>
#include <cstdint>
#include <string>

namespace Engine
{
	struct DeviceInput
	{
		uint8_t deviceID;
		uint16_t layoutID;
		InputTypes type;
	};

	struct InputDevice
	{
		std::string displayName;
		uint8_t id;
		std::unordered_map<std::string, uint16_t> layouts;
	};
}