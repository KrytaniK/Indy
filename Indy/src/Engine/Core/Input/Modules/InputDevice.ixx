module;

#include <string>
#include <memory>
#include <vector>
#include <functional>

export module Indy.Input:Device;

import :State;
import :Control;

export
{
	namespace Indy
	{
		// Describes the memory layout of a physical input device.
		struct InputLayout
		{
			std::string displayName;
			uint16_t deviceClass = 0xFFFF; // 2-byte hex value (0-65535) used to classify a device.
			uint16_t layoutClass = 0xFFFF; // 2-byte hex value (0-65535) used to classify a layout.
			uint16_t sizeInBytes = 0x0000; // 2-byte hex value (0-65535) used to store the total memory size of this layout
			std::vector<InputControlInfo> controls; // Specific control information
		};

		// Describes a physical input device
		struct InputDeviceInfo
		{
			std::string displayName;
			uint16_t deviceClass = 0xFFFF; // 2-byte hex value (0-65535) used to classify a device.
			uint16_t layoutClass = 0xFFFF; // [Optional] 2-byte hex value (0-65535) used to classify the desired layout.
		};

		class InputDevice
		{
		public:
			InputDevice(const InputDeviceInfo& info, const uint16_t stateSize);
			~InputDevice() = default;

			const InputDeviceInfo& GetInfo() const;

			void AddControl(InputControl control);
			void WatchControl(const std::string& controlName, std::function<void(InputControlContext&)>& callback);

			void UpdateDeviceState(std::byte* newState);
			void UpdateControlState(const std::string& controlName, std::byte* data);

		private:
			InputDeviceInfo m_Info;
			std::vector<InputControl> m_Controls;
			std::shared_ptr<InputState> m_State;
		};
	}
}