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

	DeviceControl::DeviceControl(const DeviceControlInfo& info, const std::vector<std::shared_ptr<DeviceControl>>& childControls)
		: m_Info(info), m_Children(childControls)
	{

	}

	const DeviceControlInfo& DeviceControl::GetInfo() const
	{
		return m_Info;
	}

	std::weak_ptr<DeviceControl> DeviceControl::GetChild(const std::string& controlName)
	{
		for (const auto& child : m_Children)
		{
			if (child->GetInfo().displayName == controlName)
				return child;
		}

		return std::weak_ptr<DeviceControl>();
	}

	std::weak_ptr<DeviceControl> DeviceControl::GetChild(uint16_t index)
	{
		if (index < m_Children.size())
			return m_Children[index];

		return std::weak_ptr<DeviceControl>();
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