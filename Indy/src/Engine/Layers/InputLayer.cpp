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
				ProcessInput(*static_cast<InputNotifyInfo*>(event.data));
				break;
			}
			case (EventActions::Create):
			{
				InputCreateInfo createInfo = *static_cast<InputCreateInfo*>(event.data);
				
				if (createInfo.deviceLayout)
					m_DeviceManager->AddLayout(*createInfo.deviceLayout);

				if (createInfo.deviceInfo)
					m_DeviceManager->AddDevice(*createInfo.deviceInfo);

				break;
			}
			case (EventActions::Watch):
			{
				InputWatchInfo watchInfo = *static_cast<InputWatchInfo*>(event.data);

				std::weak_ptr<Device> device;

				if (watchInfo.device != "")
					device = m_DeviceManager->GetDevice(watchInfo.device);
				else if (watchInfo.layoutClass != 0xFFFF)
					device = m_DeviceManager->GetDevice(watchInfo.deviceClass, watchInfo.layoutClass);
				else
					device = m_DeviceManager->GetActiveDevice(watchInfo.deviceClass);

				if (device.expired())
				{
					INDY_CORE_ERROR("Could not watch control [{0}]. Device no longer exists!", watchInfo.control);
					return;
				}

				std::weak_ptr<DeviceControl> control = device.lock()->GetControl(watchInfo.control);

				if (control.expired())
				{
					INDY_CORE_ERROR("Could not watch control [{0}]. Control does not exist!", watchInfo.control);
					return;
				}

				control.lock()->Watch(watchInfo.callback);

				break;
			}
			default:
				break;
		}
	}

	void InputLayer::ProcessInput(const InputNotifyInfo& notif)
	{
		// Retrieve device
		std::weak_ptr<Device> device = notif.device != "" ?
			m_DeviceManager->GetDevice(notif.device) :
			m_DeviceManager->GetDevice(notif.deviceClass, notif.layoutClass);

		// Ensure memory hasn't been released
		if (device.expired())
		{
			INDY_CORE_ERROR("Failed to process input. Device does not exists... \n[Device Name] {0}\n[Device Class] {1}\n[Device Layout] {2}", notif.device, notif.deviceClass, notif.layoutClass);
			return;
		}

		// If the update is effecting the whole device state, write to entire device state
		if (notif.isComplete)
		{
			// Get device state, ensuring the memory hasn't been released.
			std::weak_ptr<DeviceState> deviceState = device.lock()->GetState();
			if (deviceState.expired())
			{
				INDY_CORE_ERROR("Failed to process input. Device state has expired... \n[Device Class] {0}\n[Device Layout] {1}", notif.deviceClass, notif.layoutClass);
				return;
			}

			// Overwrite full device state
			deviceState.lock()->Write(0, static_cast<std::byte*>(notif.data), deviceState.lock()->Size());
			return;
		}

		// Otherwise, Retrieve the control
		std::weak_ptr<DeviceControl> control = device.lock()->GetControl(notif.control);

		// Ensure memory hasn't been released
		if (control.expired())
		{
			INDY_CORE_ERROR("Failed to process input. Device Control does not exists... \n[Control Name] {0}\n", notif.control);
			return;
		}

		// Update control state
		control.lock()->Update(static_cast<std::byte*>(notif.data));
	}
}