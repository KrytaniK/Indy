module;

#include <stdint.h>
#include <string>
#include <vector>
#include <memory>

export module Indy_Core_Input:DeviceControl;

import :DeviceState;

export
{
	namespace Indy
	{
		struct DeviceControlInfo
		{
			std::string displayName;
			uint16_t sizeInBytes = 0xFFFF; // Control size in bytes
			uint16_t sizeInBits = 0xFFFF; // Control size in bits

			uint16_t byteOffset = 0xFFFF; // Byte offset of this control in device state
			uint8_t bit = 0xFF; // The bit value this control modifies.

			uint8_t childCount = 0x00; // 1-byte integer (0-255) representing the number of child controls.
		};

		class DeviceControl
		{
			friend class Device;

		public:
			DeviceControl(const DeviceControlInfo& info);
			DeviceControl(const DeviceControlInfo& info, const std::vector<std::shared_ptr<DeviceControl>>& childControls);
			~DeviceControl() = default;

			const DeviceControlInfo& GetInfo() const;
			std::weak_ptr<DeviceControl> GetChild(const std::string& controlName);
			std::weak_ptr<DeviceControl> GetChild(uint16_t index);

		private:
			void AttachTo(std::weak_ptr<DeviceState> state);

		private:
			DeviceControlInfo m_Info;
			std::vector<std::shared_ptr<DeviceControl>> m_Children;
			std::weak_ptr<DeviceState> m_State;
		};
	}
}