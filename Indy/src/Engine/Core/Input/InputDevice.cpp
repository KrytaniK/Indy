#include <Engine/Core/LogMacros.h>

#include <memory>
#include <string>

import Indy.Input;

namespace Indy
{
	InputDevice::InputDevice(const InputDeviceInfo& info, const uint16_t stateSize)
		: m_Info(info), m_State(std::make_shared<InputState>(stateSize))
	{}

	const InputDeviceInfo& InputDevice::GetInfo() const
	{
		return m_Info;
	}

	void InputDevice::AddControl(InputControl control)
	{
		// Bind control state to device state
		control.BindState(m_State.get());

		m_Controls.emplace_back(control);
	}

	void InputDevice::WatchControl(const std::string& controlName, std::function<void(InputControlContext&)>& callback)
	{
		for (auto& control : m_Controls)
		{
			if (control.GetInfo().displayName == controlName)
			{
				control.Watch(callback);
				return;
			}

			control.WatchChild(controlName, callback);
		}
	}

	void InputDevice::UpdateDeviceState(std::byte* newState)
	{
		// Bail if state is invalid
		if (!m_State)
		{
			INDY_CORE_ERROR(
				"Failed to process input. Bad State... \n[Device Name] {0}\n[Device Class] {1}\n[Device Layout] {2}",
				m_Info.displayName,
				m_Info.deviceClass,
				m_Info.layoutClass
			);
			return;
		}

		// Write device state
		m_State->Write(0, newState, m_State->Size());
	}

	void InputDevice::UpdateControlState(const std::string& controlName, std::byte* data)
	{
		// First search 
		for (auto& control : m_Controls)
		{
			if (control.GetInfo().displayName == controlName)
			{
				// Only update target control if the names match
				control.Update(data);
				return;
			}
			
			// Otherwise, attempt to update any child controls.
			// NOTE: This could potentially update multiple controls if there are more than one child controls
			//		with the same name.
			control.UpdateChild(controlName, data);
		}
	}

}