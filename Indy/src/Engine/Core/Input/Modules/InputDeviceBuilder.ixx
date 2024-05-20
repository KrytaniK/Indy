module;

#include <cstdint>
#include <vector>
#include <utility>

export module Indy.Input:DeviceBuilder;

import :Device;

export
{
	namespace Indy::Input
	{
		class DeviceBuilder
		{
		public:
			DeviceBuilder() {};
			~DeviceBuilder() = default;

			Device Build(const DeviceInfo& deviceInfo, const Layout& deviceLayout);

		private:
			uint32_t m_DeviceCount = 0;
		};
	}
}