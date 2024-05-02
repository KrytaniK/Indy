module;

#include <vector>
#include <memory>
#include <queue>

export module Indy_Core_Input:DeviceManager;

import Indy_Core_Events;

import :DeviceBuilder;
import :Device;

export
{
	namespace Indy
	{
		struct DeviceDetectEvent : IEvent
		{
			DeviceInfo deviceInfo;
		};

		class DeviceManager
		{
		public:
			DeviceManager();
			~DeviceManager();

		private:
			void OnDeviceDetected(DeviceDetectEvent& event);

			std::unique_ptr<DeviceLayout> MatchDeviceLayout(const DeviceInfo& deviceInfo);

		private:
			std::unique_ptr<DeviceBuilder> m_DeviceBuilder;
			std::vector<std::shared_ptr<Device>> m_Devices;
			std::queue<DeviceInfo> m_DeviceQueue;
			std::vector<DeviceLayout> m_Layouts;
		};
	}
}