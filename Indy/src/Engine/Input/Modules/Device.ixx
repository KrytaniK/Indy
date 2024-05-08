module;

#include <string>
#include <memory>
#include <vector>
#include <functional>

export module Indy_Core_Input:Device;

export import :DeviceState;
export import :DeviceControl;

export
{
	namespace Indy
	{
		// Describes the memory layout of a physical input device.
		struct DeviceLayout
		{
			std::string displayName;
			uint16_t deviceClass = 0xFFFF; // 2-byte hex value (0-65535) used to classify a device.
			uint16_t layoutClass = 0xFFFF; // 2-byte hex value (0-65535) used to classify a layout.
			uint16_t sizeInBytes = 0x0000; // 2-byte hex value (0-65535) used to store the total memory size of this layout
			std::vector<DeviceControlInfo> controls; // Specific control informations
		};

		// Describes a physical input device
		struct DeviceInfo
		{
			std::string displayName;
			uint16_t deviceClass = 0xFFFF; // 2-byte hex value (0-65535) used to classify a device.
			uint16_t layoutClass = 0xFFFF; // [Optional] 2-byte hex value (0-65535) used to classify the desired layout.
		};

		class Device
		{
		public:
			Device(const DeviceInfo& info, const uint16_t stateSize);
			~Device() = default;

			const DeviceInfo& GetInfo() const;

			void AddControl(std::shared_ptr<DeviceControl>& control);
			void WatchControl(const std::string& controlName, std::function<void(DeviceControlContext&)>& callback);

			void UpdateDeviceState(std::byte* newState);
			void UpdateControlState(const std::string& controlName, std::byte* data);

		private:
			DeviceInfo m_Info;
			std::vector<std::shared_ptr<DeviceControl>> m_Controls;
			std::shared_ptr<DeviceState> m_State;
		};
	}
}