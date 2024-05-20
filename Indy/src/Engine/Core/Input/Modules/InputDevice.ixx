module;

#include <string>
#include <memory>
#include <vector>
#include <cstdint>
#include <unordered_map>

export module Indy.Input:Device;

import :State;
import :Control;

export
{
	namespace Indy::Input
	{
		// Describes the memory layout of a physical input device.
		struct Layout
		{
			std::string displayName;
			uint16_t deviceClass = 0xFFFF; // 2-byte hex value (0-65535) used to classify a device.
			uint16_t id = 0xFFFF; // 2-byte hex value (0-65535) used to classify a layout.
			uint16_t sizeInBytes = 0x0000; // 2-byte hex value (0-65535) used to store the total memory size of this layout
			std::vector<ControlInfo> controls; // ordered control information
		};

		// Describes a physical input device
		struct DeviceInfo
		{
			std::string displayName;
			uint32_t id = 0xFFFFFFFF;
			uint16_t deviceClass = 0xFFFF; // 2-byte hex value (0-65535) used to classify a device.
			uint16_t layoutID = 0xFFFF; // [Optional] 2-byte hex value (0-65535) used to classify the desired layout.
		};

		class Device
		{
		public:
			Device(const DeviceInfo& info, const uint16_t stateSize);
			~Device() = default;

			const std::string& GetName() const; 
			const uint32_t& GetID() const;

			Control* AddControl(const ControlInfo& controlInfo);
			Control* GetControl(const uint32_t& id);
			Control* GetControl(const std::string& alias);

			void UpdateState(std::byte* newState);

		private:
			DeviceInfo m_Info;
			std::unordered_map<uint32_t, std::shared_ptr<Control>> m_Controls;
			std::shared_ptr<InputState> m_State;
		};
	}
}