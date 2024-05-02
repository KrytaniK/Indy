#include "Engine/Core/LogMacros.h"

#include <memory>

import Indy_Core_Input;

namespace Indy
{
	DeviceControl::DeviceControl(const DeviceControlInfo& info)
		: m_Info(info)
	{
		// Reserve enough space to store child controls
		if (info.childCount > 0)
			m_Children.reserve(info.childCount);
	}

	DeviceControl::~DeviceControl()
	{

	}

	void DeviceControl::AddChild(std::shared_ptr<DeviceControl> control)
	{
		for (const auto& child : m_Children)
		{
			if (child->GetInfo().displayName == control->GetInfo().displayName)
			{
				INDY_CORE_ERROR("Could not add control [{0}]. Control already exists!", control->GetInfo().displayName);
				return;
			}
		}

		m_Children.emplace_back(control);
	}

	const DeviceControlInfo& DeviceControl::GetInfo() const
	{
		return m_Info;
	}

	void DeviceControl::AttachTo(std::weak_ptr<DeviceState> state)
	{
		// Make sure the state is not expired
		if (state.expired())
		{
			INDY_CORE_ERROR("Failed to attach control [{0}]. Device State has expired...", m_Info.displayName);
			return;
		}

		m_State = state;
	}
}