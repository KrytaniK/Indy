#include "Engine/Core/LogMacros.h"
#include <memory>

import Indy_Core_Events;
import Indy_Core_LayerStack;
import Indy_Core_InputLayer;

namespace Indy
{
	void InputLayer::onAttach()
	{
		// Initialize the device manager
		m_DeviceManager = std::make_shared<DeviceManager>();

		// Correspond to enum class values.
		m_LayerActions.resize(4);
		m_LayerActions[0] = std::make_unique<ICL_InputAction_Update>(m_DeviceManager.get());
		m_LayerActions[1] = std::make_unique<ICL_InputAction_CreateLayout>(m_DeviceManager.get());
		m_LayerActions[2] = std::make_unique<ICL_InputAction_CreateDevice>(m_DeviceManager.get());
		m_LayerActions[3] = std::make_unique<ICL_InputAction_WatchControl>(m_DeviceManager.get());

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

	void InputLayer::onEvent(ILayerEvent* event)
	{

		if (event->targetLayer != "ICL_Input")
			return;

		event->propagates = false;

		// Based on which action is specified, execute said action
		uint8_t action = static_cast<uint8_t>(((ICL_InputEvent*)event)->action);
		m_LayerActions[action]->Execute(event->layerData);
	}
}