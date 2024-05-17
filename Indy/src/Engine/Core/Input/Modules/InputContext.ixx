module;

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <unordered_map>

export module Indy.Input:Context;

import :DeviceManager;
import :Device;
import :Control;

export
{
	namespace Indy
	{
		class InputContext
		{
		public:
			InputContext(const std::shared_ptr<InputDeviceManager>& deviceManager, const std::vector<InputDeviceInfo>& deviceInfos);
			InputContext(const std::shared_ptr<InputDeviceManager>& deviceManager, const std::vector<std::string>& deviceNames);
			~InputContext() = default;

			void OnValueChange(const std::string& deviceName, const std::string& controlName, std::function<void(InputControlContext&)> callback);

			void UpdateDevice(const std::string& deviceName, void* newState);
			void UpdateControl(const std::string& deviceName, const std::string& controlName, void* newState);

		private:
			InputContext(const InputContext&) = delete;

			InputDevice* GetDevice(const std::string& deviceName);

		private:
			std::shared_ptr<InputDeviceManager> m_DeviceManager;
			std::unordered_map<std::string, InputDevice> m_Devices;
		};
	}
}