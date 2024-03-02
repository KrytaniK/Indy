module;

#include <string>
#include <vector>
#include <memory>
#include <functional>

export module Indy_Core_InputSystem:Device;

import :DeviceControl;

export
{
	namespace Indy
	{
		// Representation of a backend device
		struct InputDeviceInfo
		{
			std::string vendorID; // Vendor-Specific string ID (e.g., "0x54C" for Sony Entertainment)
			std::string productID; // Device-Specific Product ID (e.g., "0x9CC" for wireless controller)
			std::string serialID; // Device/Driver serial number (if available)
			std::string classification; // The type of device (e.g., "Gamepad")
			std::string capabilities; // For use with external APIs, allows layout building from complex descriptions
			std::string displayName; // Display Name
			int format; // unique identifier for the format of this device
		};

		class InputDevice
		{
		private:
			struct DeviceState
			{
				char* state; // internal state
				size_t size; // state size in bytes
			};

		public:
			InputDevice(const InputDeviceInfo& info);
			~InputDevice();

			void UpdateDeviceState(char* newState, size_t offset, size_t size, int bit = -1);

			void AddControl(InputDeviceControlInfo& info);
			const InputDeviceControlInfo& GetControlInfo(const std::string& controlName);
			void WatchControl(const InputDeviceControlInfo& info, const std::function<void(const InputDeviceControlValue&)>& onValueChange);
			
			const InputDeviceInfo& GetInfo() { return this->m_Info; };
			int GetFormat() const { return this->m_Info.format; };

		private:
			InputDeviceInfo m_Info;
			std::vector<std::unique_ptr<InputDeviceControl>> m_Controls;
			DeviceState m_State;
		};
	}
}