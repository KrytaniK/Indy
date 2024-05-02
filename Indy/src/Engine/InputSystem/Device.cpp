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

}