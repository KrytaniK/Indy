module;

#include <vector>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>

export module Indy.Input:DeviceManager;

import :DeviceBuilder;
import :Device;
import :Control;

export
{
	namespace Indy
	{
		class InputDeviceManager
		{
		public:
			InputDeviceManager();
			~InputDeviceManager() = default;

			void AddLayout(const InputLayout& layout);

			void AddDevice(const InputDeviceInfo& deviceInfo);
			const std::shared_ptr<InputDevice>& GetDevice(const InputDeviceInfo& deviceInfo) const;

			void UpdateDeviceState(const InputDeviceInfo& deviceInfo, const std::string& control, std::byte* data);

		private:
			std::unique_ptr<InputLayout> MatchDeviceLayout(const InputDeviceInfo& deviceInfo);

		private:
			std::unique_ptr<InputDeviceBuilder> m_DeviceBuilder;

			std::vector<std::pair<uint16_t, std::weak_ptr<InputDevice>>> m_ActiveDevices;
			std::vector<std::shared_ptr<InputDevice>> m_Devices;
			std::vector<InputLayout> m_Layouts;

			std::queue<InputDeviceInfo> m_DeviceQueue;
		};
	}
}