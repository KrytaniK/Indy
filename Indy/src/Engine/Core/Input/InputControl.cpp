#include <Engine/Core/LogMacros.h>

#include <memory>

import Indy.Input;

namespace Indy
{
	InputControl::InputControl(const InputControlInfo& info)
		: m_Info(info)
	{
		// Reserve enough space to store child controls
		if (info.childCount > 0)
			m_Children.reserve(info.childCount);
	}

	const InputControlInfo& InputControl::GetInfo() const
	{
		return m_Info;
	}

	void InputControl::BindState(InputState* state)
	{
		if (m_State)
		{
			INDY_CORE_ERROR("Could not bind state for [{0}]. Already bound to device state.", m_Info.displayName);
			return;
		}

		m_State = state;
	}

	void InputControl::AddChild(const InputControlInfo& childInfo)
	{
		InputControl child(childInfo);

		// Bind child control state to device state
		child.BindState(m_State);

		m_Children.emplace_back(child);
	}

	void InputControl::Update(std::byte* data)
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
			m_State->WriteBit(m_Info.byteOffset, m_Info.bit, (*data != std::byte{ 0 }));
		else // Otherwise, update the control state.
			m_State->Write(m_Info.byteOffset, data, m_Info.sizeInBytes);
	}

	void InputControl::UpdateChild(const std::string& childName, std::byte* data)
	{
		for (auto& child : m_Children)
		{
			if (child.GetInfo().displayName == childName)
			{
				child.Update(data);
				return;
			}
		}
	}
}