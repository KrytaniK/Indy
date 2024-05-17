module;

#include <Engine/Core/LogMacros.h>

#include <stdint.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>

export module Indy.Input:Control;

import :State;

export
{
	namespace Indy
	{
		class InputControlContext;

		struct InputControlInfo
		{
			std::string displayName;
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
			void SetParent(InputControl* parent);
			void BindState(InputState* state);
			void AddChild(const InputControlInfo& childInfo);
			void Update(std::byte* data);
			void UpdateChild(const std::string& childName, std::byte* data);

			void Watch(std::function<void(InputControlContext&)>& callback);
			void WatchChild(const std::string& childName, std::function<void(InputControlContext&)>& callback);

			template<typename T>
			T ReadAs();

		private:
			void OnValueChange();

		private:
			InputControlInfo m_Info;
			InputState* m_State = nullptr; // Associative reference to the owning device's state
			InputControl* m_ParentControl = nullptr; // Associative reference to this control's owning control.
			std::vector<InputControl> m_Children; // Vector of child controls, managed by this control.
			std::vector<std::function<void(InputControlContext&)>> m_Listeners; // Event listeners for this control
		};

		class InputControlContext
		{
		public:
			InputControlContext(InputControl* control);
			~InputControlContext() = default;

			const std::string& Name();

			template<typename T>
			T ReadAs();

		private:
			InputControl* m_Control;
		};

		// Template Definitions
		// --------------------

		template<typename T>
		T InputControl::ReadAs()
		{
			if (!m_State)
			{
				INDY_CORE_ERROR("Could not read data for control [{0}]. Invalid state.");
				return (T)(0);
			}

			if (m_Info.sizeInBits == 1)
			{
				return static_cast<T>(m_State->ReadBit(m_Info.byteOffset, m_Info.bit));
			}

			return m_State->Read<T>(m_Info.byteOffset);
		}

		InputControlContext::InputControlContext(InputControl* control)
		{
			m_Control = control;
		};

		const std::string& InputControlContext::Name()
		{
			if (!m_Control)
				return "[-- Expired --]";

			return m_Control->GetInfo().displayName;
		};

		template<typename T>
		T InputControlContext::ReadAs()
		{
			if (!m_Control)
				return T();

			return m_Control->ReadAs<T>();
		};
	}
}