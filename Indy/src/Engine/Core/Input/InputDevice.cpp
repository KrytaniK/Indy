#include <Engine/Core/LogMacros.h>

#include <memory>
#include <string>

import Indy.Input;

namespace Indy::Input
{
	Device::Device(const DeviceInfo& info, const uint16_t stateSize)
		: m_Info(info), m_State(std::make_shared<InputState>(stateSize))
	{}

	const std::string& Device::GetName() const
	{
		return m_Info.displayName;
	}

	const uint32_t& Device::GetID() const
	{
		return m_Info.id;
	}

	Control* Device::AddControl(const ControlInfo& controlInfo)
	{
		if (m_Controls.contains(controlInfo.id))
		{
			INDY_CORE_ERROR("Could not add control to device [{0}]. Control with matching ID already exists!", m_Info.displayName);
			return nullptr;
		}

		m_Controls[controlInfo.id] = std::make_shared<Control>(controlInfo);

		// Bind control state to device state
		m_Controls[controlInfo.id]->BindState(m_State);
		return m_Controls[controlInfo.id].get();
	}

	Control* Device::GetControl(const uint32_t& id)
	{
		Control* control = nullptr;

		for (auto& pair : m_Controls)
		{
			if (pair.second->GetID() == id)
				return pair.second.get();

			control = pair.second->GetChild(id);
			if (control)
				return control;
		}

		INDY_CORE_ERROR("Could not find control");

		return nullptr;
	}

	Control* Device::GetControl(const std::string& alias)
	{
		Control* control = nullptr;

		for (auto& pair : m_Controls)
		{
			if (alias == pair.second->GetName() || alias == pair.second->GetAlias())
				return pair.second.get();

			control = pair.second->GetChild(alias);

			if (control)
				return control;
		}

		INDY_CORE_ERROR("Could not find control");

		return nullptr;
	}

	const std::shared_ptr<InputState>& Device::GetState()
	{
		return m_State;
	}

	void Device::UpdateState(std::byte* newState)
	{
		// Write device state
		m_State->Write(0, newState, m_State->Size());
	}
}