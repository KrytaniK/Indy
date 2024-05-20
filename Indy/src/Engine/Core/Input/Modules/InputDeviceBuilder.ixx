module;

#include <vector>
#include <memory>
#include <cstdint>
#include <unordered_map>

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

		private:
			uint32_t m_DeviceCount = 0;
		};
	}
}