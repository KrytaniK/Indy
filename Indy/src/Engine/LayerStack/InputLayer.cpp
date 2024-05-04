#include "Engine/Core/LogMacros.h"
#include <memory>

import Indy_Core_Events;
import Indy_Core_LayerStack;
import Indy_Core_InputLayer;
import Indy_Core_Input;

namespace Indy
{
	void InputLayer::onAttach()
	{
		// Initialize the device manager
		m_DeviceManager = std::make_shared<DeviceManager>();

		// Correspond to enum class values.
		m_LayerActions.resize(4);
		m_LayerActions[0] = std::make_unique<LA_InputUpdate>();
		m_LayerActions[1] = std::make_unique<LA_InputCreateDevice>();
		m_LayerActions[2] = std::make_unique<LA_InputCreateLayout>();
		m_LayerActions[3] = std::make_unique<LA_InputWatchControl>();

		// Attach event handles
		m_EventHandles.emplace_back(
			EventManagerCSR::AddEventListener<InputLayer, ILayerEvent>(this, &InputLayer::onEvent)
		);
	}

	void InputLayer::onDetach()
	{

	}

	void InputLayer::Update()
	{

	}

	void InputLayer::onEvent(ILayerEvent& event)
	{
		INDY_CORE_INFO("Input OnEvent. Target: [{0}]", event.targetLayer);
		if (event.targetLayer != "ICL_Input")
		{
			return;
		}


		event.propagates = false;

		// Cast to Input Layer Event
		InputLayerEvent inputEvent = static_cast<InputLayerEvent>(event);

		// Attach the device manager to the action data
		InputActionData actionData;
		actionData.deviceManager = m_DeviceManager;

		// Based on which action is specified, execute said action
		uint8_t action = static_cast<uint8_t>(inputEvent.action);
		INDY_CORE_INFO("Action: {0}", action);
		return;
		m_LayerActions[action]->Execute(&actionData);
	}
}