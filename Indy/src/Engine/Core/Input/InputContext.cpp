
#include <memory>
#include <cstdint>
#include <string>
#include <functional>

#include "Engine/Core/LogMacros.h"

import Indy.Input;

import Indy.Events;

namespace Indy::Input
{
	// -----------------------------------------------------------------
	// StateContext - For reading the value after a change occurred
	// -----------------------------------------------------------------
	
	StateContext::StateContext(Control* control, const ::std::shared_ptr<InputState>& state)
		: m_Control(control), m_State(state) {}

	const uint16_t& StateContext::GetSize()
	{
		return m_Control->GetSize();
	}

	const uint32_t& StateContext::GetID()
	{
		return m_Control->GetID();
	}

	const std::string& StateContext::GetName()
	{
		return m_Control->GetName();
	}
	
	const std::string& StateContext::GetAlias()
	{
		return m_Control->GetAlias();
	}

	// ------------------------------------------------------------
	// Context - For managing reactions to input state changes
	// ------------------------------------------------------------

	Context::Context()
		:m_CallbackEvent({}) {}

	std::shared_ptr<EventDelegate> Context::AddInputCallback(const uint32_t& deviceID, const uint32_t& controlID, const std::function<void(CallbackEvent*)>& callback)
	{
		return m_InputCallbacks[deviceID][controlID].Subscribe(callback);
	}

	std::shared_ptr<EventDelegate> Context::AddInputCallback(const std::string& deviceName,
		const std::string& controlName, const std::function<void(CallbackEvent*)>& callback)
	{
		const Control* control = nullptr;
		
		DeviceGetEvent event;
		event.device_name = deviceName;

		Events<DeviceGetEvent>::Notify(&event);

		if (!event.outDevice)
		{
			INDY_CORE_ERROR("Failed to add input callback. No such device exists [{0}]", deviceName);
			return nullptr;
		}

		control = event.outDevice->GetControl(controlName);

		if (!control)
		{
			INDY_CORE_ERROR("Failed to add input callback. No such device control exists [{0}]", controlName);
			return nullptr;
		}

		INDY_CORE_WARN("Setting Callback");
		return m_InputCallbacks[event.outDevice->GetID()][control->GetID()].Subscribe(callback);
	}

	void Context::OnInput(const uint32_t& deviceID, const uint32_t& controlID, StateContext& ctx)
	{
		if (!m_InputCallbacks.contains(deviceID))
			return;

		if (!m_InputCallbacks[deviceID].contains(controlID))
			return;

		m_CallbackEvent.context = &ctx;
		m_InputCallbacks[deviceID][controlID].Notify(&m_CallbackEvent);
	}


}