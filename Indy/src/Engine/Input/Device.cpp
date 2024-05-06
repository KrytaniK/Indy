#include "Engine/Core/LogMacros.h"

#include <memory>
#include <string>

import Indy_Core_Input;

namespace Indy
{
	Device::Device(const DeviceInfo& info, const uint16_t stateSize, const std::vector<std::shared_ptr<DeviceControl>>& controls)
		: m_Info(info), m_State(std::make_shared<DeviceState>(stateSize)), m_Controls(controls)
	{
		for (const auto& control : m_Controls)
		{
			control->AttachTo(m_State);

			if (control->GetInfo().childCount > 0)
			{
				for (const auto& child : control->m_Children)
				{
					child->AttachTo(m_State);
				}
			}
		}
	}

	const DeviceInfo& Device::GetInfo() const
	{
		return m_Info;
	}

	std::weak_ptr<DeviceControl> Device::GetControl(const std::string controlName)
	{
		for (const auto& control : m_Controls)
		{
			if (control->GetInfo().displayName == controlName)
				return control;

			if (control->GetInfo().childCount > 0)
			{
				for (const auto& child : control->m_Children)
				{
					if (child->GetInfo().displayName == controlName)
						return child;
				}
			}
		}

		INDY_CORE_WARN("Requested control [{0}] does not exist...", controlName);
		return std::weak_ptr<DeviceControl>();
	}

	std::weak_ptr<DeviceControl> Device::GetControl(const uint16_t controlIndex)
	{
		if (controlIndex < m_Controls.size())
			return m_Controls[controlIndex];

		return std::weak_ptr<DeviceControl>();
	}

	std::weak_ptr<DeviceState> Device::GetState()
	{
		return m_State;
	}

	void Device::Update(std::byte* newState)
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
		return;
	}

}