#include <Engine/Core/LogMacros.h>

#include <memory>

import Indy.Input;

namespace Indy
{
	DeviceControl::DeviceControl(const DeviceControlInfo& info)
		: m_Info(info)
	{
		// Reserve enough space to store child controls
		if (info.childCount > 0)
			m_Children.reserve(info.childCount);
	}

	const DeviceControlInfo& DeviceControl::GetInfo() const
	{
		return m_Info;
	}

	void DeviceControl::SetParent(DeviceControl* parent)
	{
		m_ParentControl = parent;
	}

	void DeviceControl::BindState(DeviceState* state)
	{
		if (m_State)
		{
			INDY_CORE_ERROR("Could not bind state for [{0}]. Already bound to device state.", m_Info.displayName);
			return;
		}

		m_State = state;
	}

	void DeviceControl::AddChild(const DeviceControlInfo& childInfo)
	{
		std::shared_ptr<DeviceControl> child = std::make_shared<DeviceControl>(childInfo);

		// Bind child control state to device state
		child->BindState(m_State);

		m_Children.emplace_back(child);
	}

	void DeviceControl::OnValueChange()
	{
		// Notify all control watchers
		DeviceControlContext ctx(this);
		for (const auto& callback : m_Listeners)
			callback(ctx);

		// If no parent exists, bail
		if (!m_ParentControl)
			return;

		// Notify all parent watchers
		m_ParentControl->OnValueChange();
	}

	void DeviceControl::Update(std::byte* data)
	{
		// Can't update state if it's invalid
		if (!m_State)
		{
			INDY_CORE_ERROR("Could not set control state [{0}]. Invalid device state.", m_Info.displayName);
			return;
		}

		// If this control updates a bit
		// Data will always be converted to a boolean value of 0 or 1 before writing to state.
		if (m_Info.bit != 0xFF)
			m_State->WriteBit(m_Info.byteOffset, m_Info.bit, (*data != std::byte{0}));
		else // Otherwise, update the control state.
			m_State->Write(m_Info.byteOffset, data, m_Info.sizeInBytes);

		OnValueChange();
	}

	void DeviceControl::UpdateChild(const std::string& childName, std::byte* data)
	{
		for (const auto& child : m_Children)
		{
			if (child->GetInfo().displayName == childName)
			{
				child->Update(data);
				return;
			}
		}
	}

	void DeviceControl::Watch(std::function<void(DeviceControlContext&)>& callback)
	{
		m_Listeners.emplace_back(callback);
	}

	void DeviceControl::WatchChild(const std::string& childName, std::function<void(DeviceControlContext&)>& callback)
	{
		for (const auto& child : m_Children)
		{
			if (child->GetInfo().displayName == childName)
			{
				child->Watch(callback);
				return;
			}
		}
	}


}