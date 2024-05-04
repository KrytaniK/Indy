#include <Engine/Core/LogMacros.h>

#include <memory>
#include <iostream>
#include <vector>

import Sandbox;
import Indy_Core;
import Indy_Core_Input;
import Indy_Core_Events;

namespace Indy
{

	std::unique_ptr<Application> CreateApplication()
	{
		return std::make_unique<Sandbox>();
	};

	Sandbox::Sandbox()
	{
		m_LayerStack->PushLayer(std::make_shared<InputLayer>());

		DeviceLayout mouseLayout;
		mouseLayout.displayName = "Mouse";
		mouseLayout.deviceClass = 0x0000;
		mouseLayout.layoutClass = 0x0000;
		mouseLayout.sizeInBytes = 9;
		mouseLayout.controls = {
			{"Left Mouse Button", 0x0001, 0x0001, 0, 0, 0},
			{"Position", 0x0008, 0xFFFF, 0, 0xFF, 2},
			{"X", 0x0004, 0xFFFF, 0x0000, 0xFF, 0},
			{"Y", 0x0004, 0xFFFF, 0x0004, 0xFF, 0},
		};

		DeviceInfo deviceInfo;
		deviceInfo.displayName = "Default Mouse";
		deviceInfo.deviceClass = 0x0000;
		deviceInfo.layoutClass = 0x0000;

		InputCreateData createData;
		createData.deviceInfo = &deviceInfo;
		createData.deviceLayout = &mouseLayout;

		bool value = 1;
		float x = 37.5f;
		float pos[] = { x, 283.2f };

		InputNotifyData input;
		input.deviceClass = 0x0000;
		input.layoutClass = 0x0000;
		input.controlName = "X";
		input.data = &x;

		LayerEvent createEvent;
		createEvent.target = INDY_CORE_LAYER_INPUT;
		createEvent.action = InputLayer::EventActions::Create;
		createEvent.data = &createData;

		LayerEvent updateControlEvent;
		updateControlEvent.target = INDY_CORE_LAYER_INPUT;
		updateControlEvent.action = InputLayer::EventActions::Notify;
		updateControlEvent.data = &input;

		EventManagerCSR::Notify<LayerEvent>(createEvent);
		EventManagerCSR::Notify<LayerEvent>(updateControlEvent);
	}

	Sandbox::~Sandbox()
	{
		
	}

	void Sandbox::Run()
	{
		
	}
}