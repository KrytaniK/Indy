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

	void DeviceControl::Update(std::byte* data)
	{
		// Can't update state if it's invalid
		if (m_State.expired())
		{
			INDY_CORE_ERROR("Could not set control state [{0}]. Invalid device state.", m_Info.displayName);
			return;
		}

		// If this control updates a bit
		// Data will always be converted to a boolean value of 0 or 1 before writing to state.
		if (m_Info.bit != 0xFF)
		{

			m_State.lock()->WriteBit(m_Info.byteOffset, m_Info.bit, (*data != std::byte{0}));
			return;
		}

		// Otherwise, update the control state.
		m_State.lock()->Write(m_Info.byteOffset, data, m_Info.sizeInBytes);
	}
}