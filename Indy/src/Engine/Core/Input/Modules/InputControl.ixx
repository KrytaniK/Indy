module;

#include <Engine/Core/LogMacros.h>

#include <stdint.h>
#include <string>
#include <vector>
#include <memory>

export module Indy.Input:Control;

import :State;

export
{
	namespace Indy
	{
		struct InputControlInfo
		{
			std::string displayName; // Display name for this control
			std::vector<std::string> aliases; // alternative display names for this control
			uint32_t id; // unique ID for this control

			uint16_t sizeInBytes = 0xFFFF; // Control size in bytes
			uint16_t sizeInBits = 0xFFFF; // Control size in bits

			uint16_t byteOffset = 0xFFFF; // Byte offset of this control in device state
			uint8_t bit = 0xFF; // The bit value this control modifies.

			uint8_t childCount = 0x00; // 1-byte integer (0-255) representing the number of child controls.
		};

		class InputControl
		{
		public:
			InputControl(const InputControlInfo& info);
			~InputControl() = default;

			const InputControlInfo& GetInfo() const;

			void BindState(InputState* state);

			void AddChild(const InputControlInfo& childInfo);

			void Update(std::byte* data);
			void UpdateChild(const std::string& childName, std::byte* data);

		private:
			InputControlInfo m_Info;
			InputState* m_State = nullptr; // Associative reference to the owning device's state
			std::vector<InputControl> m_Children; // Vector of child controls, managed by this control.
		};
	}
}