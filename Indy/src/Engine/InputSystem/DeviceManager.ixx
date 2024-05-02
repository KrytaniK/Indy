module;

#include <vector>
#include <memory>
#include <queue>
#include <string>

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

			void AddLayout(const DeviceLayout& layout);

			std::weak_ptr<Device> GetDevice(const std::string& displayName);
			std::weak_ptr<Device> GetDevice(uint16_t deviceClass, uint16_t layoutClass);

			std::weak_ptr<Device> GetActiveDevice(uint16_t deviceClass);
			void SetActiveDevice(uint16_t deviceClass, const std::weak_ptr<Device>& device);

		private:
			void OnDeviceDetected(DeviceDetectEvent& event);

			std::unique_ptr<DeviceLayout> MatchDeviceLayout(const DeviceInfo& deviceInfo);

		private:
			std::unique_ptr<DeviceBuilder> m_DeviceBuilder;

			std::vector<std::pair<uint16_t, std::weak_ptr<Device>>> m_ActiveDevices;
			std::vector<std::shared_ptr<Device>> m_Devices;
			std::vector<DeviceLayout> m_Layouts;

			std::queue<DeviceInfo> m_DeviceQueue;

			std::queue<IEventHandle> m_EventHandles;
		};
	}
}