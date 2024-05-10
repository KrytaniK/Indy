module;

#include <vector>
#include <memory>
#include <queue>
#include <string>
#include <functional>

export module Indy_Core_Input:DeviceManager;

import :DeviceBuilder;
import :Device;

export
{
	namespace Indy
	{
		class DeviceManager
		{
		public:
			DeviceManager();
			~DeviceManager() = default;

			void AddLayout(const DeviceLayout& layout);
			void AddDevice(const DeviceInfo& deviceInfo);

			void UpdateDeviceState(const DeviceInfo* deviceInfo, const std::string& control, std::byte* data);

			void WatchDeviceControl(const DeviceInfo* deviceInfo, const std::string& control, std::function<void(DeviceControlContext&)>& onValueChange);

		private:
			std::unique_ptr<DeviceLayout> MatchDeviceLayout(const DeviceInfo& deviceInfo);

		private:
			std::unique_ptr<DeviceBuilder> m_DeviceBuilder;

			std::vector<std::pair<uint16_t, std::weak_ptr<Device>>> m_ActiveDevices;
			std::vector<std::shared_ptr<Device>> m_Devices;
			std::vector<DeviceLayout> m_Layouts;

			std::queue<DeviceInfo> m_DeviceQueue;
		};
	}
}