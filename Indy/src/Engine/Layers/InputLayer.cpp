#include "Engine/Core/LogMacros.h"
#include <memory>

import Indy_Core;
import Indy_Core_Events;
import Indy_Core_Input;

namespace Indy
{
	void InputLayer::onAttach()
	{
		// Initialize the device manager
		m_DeviceManager = std::make_unique<DeviceManager>();

		// Add all known layouts

		// Build all known devices

		m_EventHandles.emplace_back(
			EventManagerCSR::AddEventListener<InputLayer, LayerEvent>(this, &InputLayer::onEvent)
		);
	}

	void InputLayer::onDetach()
	{

	}

	void InputLayer::Update()
	{

	}

	void InputLayer::onEvent(LayerEvent& event)
	{
		if (event.target != INDY_CORE_LAYER_INPUT)
			return;

		event.propagates = false;

		// Handle Event
		switch (event.action)
		{
			case (EventActions::Notify):
			{
				ProcessInput(*static_cast<InputNotifyData*>(event.data));
				break;
			}
			case (EventActions::Create):
			{
				InputCreateData createData = *static_cast<InputCreateData*>(event.data);
				
				if (createData.deviceLayout)
					m_DeviceManager->AddLayout(*createData.deviceLayout);

				if (createData.deviceInfo)
					m_DeviceManager->AddDevice(*createData.deviceInfo);
			}
			default:
				break;
		}
	}

	void InputLayer::ProcessInput(const InputNotifyData& eventData)
	{
		// Retrieve device
		std::weak_ptr<Device> device = m_DeviceManager->GetDevice(eventData.deviceClass, eventData.layoutClass);

		// Ensure memory hasn't been released
		if (device.expired())
		{
			INDY_CORE_ERROR("Failed to process input. Device does not exists... \n[Device Class] {0}\n[Device Layout] {1}", eventData.deviceClass, eventData.layoutClass);
			return;
		}

		// If the update is effecting the whole device state, write to entire device state
		if (eventData.isComplete)
		{
			// Get device state, ensuring the memory hasn't been released.
			std::weak_ptr<DeviceState> deviceState = device.lock()->GetState();
			if (deviceState.expired())
			{
				INDY_CORE_ERROR("Failed to process input. Device state has expired... \n[Device Class] {0}\n[Device Layout] {1}", eventData.deviceClass, eventData.layoutClass);
				return;
			}

			// Overwrite full device state
			//deviceState.lock()->Write(0, event.data, event.size);
			return;
		}

		// Otherwise, Retrieve the control
		std::weak_ptr<DeviceControl> control;
		if (eventData.controlIndex != 0xFFFF)
			control = device.lock()->GetControl(eventData.controlIndex);
		else if (eventData.controlName != "")
			control = device.lock()->GetControl(eventData.controlName);

		// Ensure memory hasn't been released
		if (control.expired())
		{
			INDY_CORE_ERROR("Failed to process input. Device Control does not exists... \n[Control Name] {0}\n[Control Index] {1}", eventData.controlName, eventData.controlIndex);
			return;
		}

		// Update control state
		control.lock()->Update(static_cast<std::byte*>(eventData.data));
	}
}