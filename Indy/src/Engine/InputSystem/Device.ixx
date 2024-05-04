module;

#include <string>
#include <memory>
#include <vector>

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
			Device(const DeviceInfo& info, const uint16_t stateSize, const std::vector<std::shared_ptr<DeviceControl>>& controls);
			~Device() = default;

			const DeviceInfo& GetInfo() const;

			std::weak_ptr<DeviceControl> GetControl(const std::string controlName);
			std::weak_ptr<DeviceControl> GetControl(const uint16_t controlIndex);

			std::weak_ptr<DeviceState> GetState();

		private:
			DeviceInfo m_Info;
			std::vector<std::shared_ptr<DeviceControl>> m_Controls;
			std::shared_ptr<DeviceState> m_State;
		};
	}
}