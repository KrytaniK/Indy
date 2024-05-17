module;

#include <vector>
#include <memory>

export module Indy.Input:DeviceBuilder;

import :Device;

export
{
	namespace Indy
	{
		class InputDeviceBuilder
		{
		public:
			InputDeviceBuilder() {};
			~InputDeviceBuilder() = default;

			std::shared_ptr<InputDevice> Build(const InputDeviceInfo& deviceInfo, const InputLayout& deviceLayout);
			std::vector<std::shared_ptr<InputDevice>> Build(const std::vector<std::pair<const InputDeviceInfo&, const InputLayout&>>& deviceList);
		};
	}
}