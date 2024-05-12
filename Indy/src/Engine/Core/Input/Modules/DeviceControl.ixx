module;

#include <Engine/Core/LogMacros.h>

#include <stdint.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>

export module Indy.Input:DeviceControl;

import :DeviceState;

export
{
	namespace Indy
	{
		class DeviceControlContext;

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
		public:
			DeviceControl(const DeviceControlInfo& info);
			~DeviceControl() = default;

			const DeviceControlInfo& GetInfo() const;
			void SetParent(DeviceControl* parent);
			void BindState(DeviceState* state);
			void AddChild(const DeviceControlInfo& childInfo);
			void Update(std::byte* data);
			void UpdateChild(const std::string& childName, std::byte* data);

			void Watch(std::function<void(DeviceControlContext&)>& callback);
			void WatchChild(const std::string& childName, std::function<void(DeviceControlContext&)>& callback);

			template<typename T>
			T ReadAs();

		private:
			DeviceControl(const DeviceControl&) = delete; // Remove Copy Constructor.
			void OnValueChange();

		private:
			DeviceControlInfo m_Info;
			DeviceState* m_State = nullptr; // Associative reference to the owning device's state
			DeviceControl* m_ParentControl = nullptr; // Associative reference to this control's owning control.
			std::vector<std::shared_ptr<DeviceControl>> m_Children; // Vector of child controls, managed by this control.
			std::vector<std::function<void(DeviceControlContext&)>> m_Listeners; // Event listeners for this control
		};

		class DeviceControlContext
		{
		public:
			DeviceControlContext(DeviceControl* control);
			~DeviceControlContext() = default;

			const std::string& Name();

			template<typename T>
			T ReadAs();

		private:
			DeviceControl* m_Control;
		};

		// Template Definitions
		// --------------------

		template<typename T>
		T DeviceControl::ReadAs()
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

		DeviceControlContext::DeviceControlContext(DeviceControl* control)
		{
			m_Control = control;
		};

		const std::string& DeviceControlContext::Name()
		{
			if (!m_Control)
				return "[-- Expired --]";

			return m_Control->GetInfo().displayName;
		};

		template<typename T>
		T DeviceControlContext::ReadAs()
		{
			if (!m_Control)
				return T();

			return m_Control->ReadAs<T>();
		};
	}
}