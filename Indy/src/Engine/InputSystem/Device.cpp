#include "Engine/Core/LogMacros.h"

#include <memory>
#include <string>

import Indy_Core_Input;

namespace Indy
{
	Device::Device(const DeviceInfo& info, const uint16_t stateSize)
		: m_Info(info), m_State(std::make_shared<DeviceState>(stateSize))
	{
	}

	const DeviceInfo& Device::GetInfo() const
	{
		return m_Info;
	}

	void Device::AddControl(std::shared_ptr<DeviceControl> control)
	{
		// Guard against existing controls
		for (const auto& deviceControl : m_Controls)
		{
			if (deviceControl->GetInfo().displayName == control->GetInfo().displayName)
			{
				INDY_CORE_ERROR("Could not add control [{0}]. Control already exists!", control->GetInfo().displayName);
				return;
			}
		}

		// Give the control a weak pointer to device state
		control->AttachTo(m_State);

		// Store the device control
		m_Controls.emplace_back(control);
	}

	std::weak_ptr<DeviceControl> Device::GetControl(const std::string controlName)
	{
		for (const auto& control : m_Controls)
		{
			if (control->GetInfo().displayName == controlName)
				return control;
		}
	}

}