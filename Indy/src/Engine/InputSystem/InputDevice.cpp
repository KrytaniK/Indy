#include "Engine/Core/LogMacros.h"
#include <memory>

import Indy_Core_InputSystem;

namespace Indy
{
	InputDevice::InputDevice(const InputDeviceInfo& info) : m_State({nullptr, 0}), m_Info(info)
	{
		
	}

	InputDevice::~InputDevice()
	{
		delete[] m_State.state;
	}

	void InputDevice::UpdateDeviceState(char* newState, size_t offset, size_t size, int bit)
	{
		if (bit < 0 && (m_State.state == nullptr || m_State.size == size)) // Full device state update is required
		{
			m_State.state = newState;
			m_State.size = size;
			return;
		}

		if (size <= 0)
		{
			INDY_CORE_ERROR("[InputDevice::UpdateDeviceState] Invalid Size. New device state must be larger than 0 bytes!");
			return;
		}

		if (offset < 0 || offset + size > m_State.size)
		{
			INDY_CORE_ERROR(
				"[InputDevice::UpdateDeviceState] Out of bounds! Offset [{0}] must be greater than 0, and Offset + size [{1}] must be less than the total size of device state {2}",
				offset, offset + size, m_State.size
			);
			return;
		}

		if (bit >= 0)
		{
			if (bit >= 8)
			{
				offset += bit / 8;
				bit = bit % 8;
			}

			bool value = (bool)*newState;
			if (value)
			{
				m_State.state[offset] |= (1u << bit);
			}
			else
			{
				m_State.state[offset] &= ~(1u << bit);
			}
		}
		else
		{
			m_State.state += offset;
			memcpy(m_State.state, newState, size);
			m_State.state -= offset;
		}
		
		// Notify the controls associated with each portion of data
		for (auto& control : m_Controls)
		{
			const auto& info = control->GetInfo();

			if (info.offset >= offset && info.offset < offset + size)
			{
				if (bit >= 0)
				{
					if (info.bit == bit)
					{
						control->Update();
					}

					continue;
				}

				control->Update();
			}
		}
	}

	void InputDevice::AddControl(InputDeviceControlInfo& info)
	{
		info.deviceFormat = m_Info.format;
		m_Controls.emplace_back(std::make_unique<InputDeviceControl>(info, this->m_State.state));
	}

	const InputDeviceControlInfo& InputDevice::GetControlInfo(const std::string& controlName)
	{
		for (auto& control : m_Controls)
			if (control->GetInfo().displayName == controlName)
				return control->GetInfo();

		INDY_CORE_CRITICAL("Device Control [{0}] does not exist!", controlName);
		throw std::runtime_error("Could not find device control.");
	}

	void InputDevice::WatchControl(const InputDeviceControlInfo& info, const std::function<void(const InputDeviceControlValue&)>& onValueChange)
	{
		for (auto& control : m_Controls)
			if (control->GetInfo().displayName == info.displayName)
			{
				control->Watch(onValueChange);
				return;
			}

		INDY_CORE_ERROR("Could not find device control...");
	}
}