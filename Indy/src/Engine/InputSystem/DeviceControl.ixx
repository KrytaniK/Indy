module;

#include "Engine/Core/LogMacros.h"
#include <stdint.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>

export module Indy_Core_Input:DeviceControl;

import :DeviceState;

// TODO: I need a way to deduce type information for each control

export
{
	namespace Indy
	{
		class DeviceControlContext;

		typedef std::function<void(DeviceControlContext& ctx)> ControlContextCallback;

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
			DeviceControl(const DeviceControlInfo& info, const std::vector< std::shared_ptr<DeviceControl>>& childControls);
			~DeviceControl() = default;

			void SetParent(const std::weak_ptr<DeviceControl>& parent);

			const DeviceControlInfo& GetInfo() const;

			std::weak_ptr<DeviceControl> GetChild(const std::string& controlName);
			std::weak_ptr<DeviceControl> GetChild(uint16_t index);

			void Update(std::byte* data);

			void Watch(ControlContextCallback callback);

			template<typename T>
			T ReadAs()
			{
				if (m_State.expired())
				{
					INDY_CORE_ERROR("Could not read data for control [{0}]. Invalid state.");
					return (T)(0);
				}

				if (m_Info.bit != 0xFF)
				{
					return static_cast<T>(m_State.lock()->ReadBit(m_Info.byteOffset, m_Info.bit));
				}

				return m_State.lock()->Read<T>(m_Info.byteOffset);
			}

		private:
			void AttachTo(std::weak_ptr<DeviceState> state);
			void OnValueChange();

		private:
			DeviceControlInfo m_Info;
			std::vector<std::shared_ptr<DeviceControl>> m_Children;
			std::vector<ControlContextCallback> m_Listeners;
			std::weak_ptr<DeviceControl> m_ParentControl;
			std::weak_ptr<DeviceState> m_State;
		};

		class DeviceControlContext
		{
		public:
			DeviceControlContext(DeviceControl* control)
			{
				m_Control = control;
			};

			~DeviceControlContext() = default;

			const std::string& Name()
			{
				if (!m_Control)
					return "[Expired]";

				return m_Control->GetInfo().displayName;
			};

			template<typename T>
			T ReadAs()
			{
				if (!m_Control)
					return T();

				return m_Control->ReadAs<T>();
			};

		private:
			DeviceControl* m_Control;
		};
	}
}