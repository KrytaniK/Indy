#include <Engine/Core/LogMacros.h>

#include <memory>

import Indy.Input;

namespace Indy::Input
{
	Control::Control(const ControlInfo& info)
		: m_Info(info) {}

	const ControlInfo& Control::GetInfo() const
	{
		return m_Info;
	}

	const std::string& Control::GetName() const
	{
		return m_Info.displayName;
	}
	
	const std::string& Control::GetAlias() const
	{
		return m_Info.alias;
	}

	const uint32_t& Control::GetID() const
	{
		return m_Info.id;
	}

	const uint16_t& Control::GetSize() const
	{
		return m_Info.sizeInBytes;
	}

	Control* Control::Get()
	{
		return this;
	}

	Control* Control::GetChild(const uint32_t& id)
	{
		auto it = m_Children.find(id);

		if (it == m_Children.end())
			return nullptr;

		return it->second.get();
	}

	Control* Control::GetChild(const std::string& alias)
	{
		for (auto& pair : m_Children)
		{
			if (pair.second->GetName() == alias || pair.second->GetAlias() == alias)
				return pair.second.get();
		}

		return nullptr;
	}

	void Control::BindState(const std::shared_ptr<InputState>& state)
	{
		if (m_State)
		{
			INDY_CORE_ERROR("Could not bind state for [{0}]. Already bound to device state.", m_Info.displayName);
			return;
		}

		m_State = state;
	}

	void Control::AddChild(const ControlInfo& childInfo)
	{
		if (m_Children.contains(childInfo.id))
		{
			INDY_CORE_ERROR("Could not add child control to [{0}]. Control already exists with a matching ID!", m_Info.displayName);
			return;
		}

		m_Children[childInfo.id] = std::make_shared<Control>(childInfo);
		m_Children[childInfo.id]->BindState(m_State);
	}

	void Control::Update(std::byte* data)
	{
		// Can't update state if it's invalid
		if (!m_State)
		{
			INDY_CORE_ERROR("Could not set control state [{0}]. Invalid device state.", m_Info.displayName);
			return;
		}

		// If this control updates a single bit
		// Data will always be converted to a boolean value of 0 or 1 before writing to state.
		if (m_Info.bit != 0xFF)
			m_State->WriteBit(m_Info.byteOffset, m_Info.bit, (*data != std::byte{ 0 }));
		else // Otherwise, update the control state.
			m_State->Write(m_Info.byteOffset, data, m_Info.sizeInBytes);
	}
}