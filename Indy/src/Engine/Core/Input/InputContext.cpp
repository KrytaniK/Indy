
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
	// InputStateContext - For reading the value after a change occurred
	// -----------------------------------------------------------------
	
	InputStateContext::InputStateContext(Control* control, const ::std::shared_ptr<InputState>& state)
		: m_Control(control), m_State(state) {}

	const uint16_t& InputStateContext::GetSize()
	{
		return m_Control->GetSize();
	}

	const uint32_t& InputStateContext::GetID()
	{
		return m_Control->GetID();
	}

	const std::string& InputStateContext::GetName()
	{
		return m_Control->GetName();
	}
	
	const std::string& InputStateContext::GetAlias()
	{
		return m_Control->GetAlias();
	}

	// ------------------------------------------------------------
	// InputContext - For managing reactions to input state changes
	// ------------------------------------------------------------

	InputContext::InputContext()
		:m_CallbackEvent({}) {}

	std::shared_ptr<EventDelegate> InputContext::AddInputCallback(const uint32_t& deviceID, const uint32_t& controlID, const std::function<void(CallbackEvent*)>& callback)
	{
		return m_InputCallbacks[deviceID][controlID].Subscribe(callback);
	}

	std::shared_ptr<EventDelegate> InputContext::AddInputCallback(const std::string& deviceName,
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

	void InputContext::OnInput(const uint32_t& deviceID, const uint32_t& controlID, InputStateContext& ctx)
	{
		if (!m_InputCallbacks.contains(deviceID))
		{
			INDY_CORE_INFO("No Callbacks for device with id {0}", deviceID);
			return;
		}

		if (!m_InputCallbacks[deviceID].contains(controlID))
		{
			INDY_CORE_INFO("No Callbacks for control with id {0}", controlID);
			return;
		}

		m_CallbackEvent.context = &ctx;
		m_InputCallbacks[deviceID][controlID].Notify(&m_CallbackEvent);
	}


}