module;

#include <vector>
#include <memory>

export module Indy_Core_Input:DeviceBuilder;

import :Device;

export
{
	namespace Indy
	{
		class DeviceBuilder
		{
		public:
			DeviceBuilder() {};
			~DeviceBuilder() = default;

			std::shared_ptr<Device> Build(const DeviceInfo& deviceInfo, const DeviceLayout& deviceLayout);
			std::vector<std::shared_ptr<Device>> Build(const std::vector<std::pair<const DeviceInfo&, const DeviceLayout&>>& deviceList);
		};
	}
}