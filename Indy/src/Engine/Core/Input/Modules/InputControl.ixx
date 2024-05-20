module;

#include <Engine/Core/LogMacros.h>

#include <stdint.h>
#include <string>
#include <unordered_map>

export module Indy.Input:Control;

import :State;

export
{
	namespace Indy::Input
	{
		struct ControlInfo
		{
			std::string displayName; // Display name for this control
			std::string alias; // alternative display name for this control
			uint32_t id = 0xFFFFFFFF; // unique ID for this control

			uint16_t sizeInBytes = 0xFFFF; // Control size in bytes 
			uint16_t sizeInBits = 0xFFFF; // Control size in bits

			uint16_t byteOffset = 0xFFFF; // Byte offset of this control in device state
			uint8_t bit = 0xFF; // The bit value this control modifies.

			uint8_t childCount = 0x00; // 1-byte integer (0-255) representing the number of child controls.
		};

		class Control
		{
		public:
			Control(const ControlInfo& info);
			~Control() = default;

			const std::string& GetName() const;
			const std::string& GetAlias() const;
			const uint32_t GetID() const;

			Control* Get();
			Control* GetChild(const uint32_t& id);
			Control* GetChild(const std::string& alias);

			void BindState(const std::shared_ptr<InputState>& state);

			void AddChild(const ControlInfo& childInfo);

			void Update(std::byte* data);

		private:
			ControlInfo m_Info;
			std::shared_ptr<InputState> m_State;
			std::unordered_map<uint32_t, std::shared_ptr<Control>> m_Children;
		};
	}
}