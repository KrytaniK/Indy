#include <Engine/Core/LogMacros.h>

#include <memory>
#include <iostream>
#include <vector>

import Sandbox;
import Indy_Core;
import Indy_Core_InputLayer;
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

		InputCreateDeviceInfo createDeviceInfo;
		createDeviceInfo.deviceInfo = &deviceInfo;

		InputCreateLayoutInfo createLayoutInfo;
		createLayoutInfo.layout = &mouseLayout;

		InputWatchControlInfo watchInfo;
		watchInfo.device = "Default Mouse";
		watchInfo.control = "Position";
		watchInfo.callback = [=](DeviceControlContext& ctx)
			{
				INDY_CORE_INFO("Mouse Position Changed!");
			};

		bool value = 1;
		float x = 37.5f;
		float pos[] = { x, 283.2f };
		void* data = &pos;

		InputUpdateInfo input;
		input.device = "Default Mouse";
		input.control = "Position";
		input.newState = &data;

		InputLayerEvent deviceCreateEvent;
		deviceCreateEvent.targetLayer = "ICL_Input";
		deviceCreateEvent.action = InputLayerAction::CreateDevice;
		deviceCreateEvent.layerData = &createDeviceInfo;

		InputLayerEvent layoutCreateEvent;
		layoutCreateEvent.targetLayer = "ICL_Input";
		layoutCreateEvent.action = InputLayerAction::CreateLayout;
		layoutCreateEvent.layerData = &createLayoutInfo;

		InputLayerEvent watchControlEvent;
		watchControlEvent.targetLayer = "ICL_Input";
		watchControlEvent.action = InputLayerAction::WatchControl;
		watchControlEvent.layerData = &watchInfo;

		InputLayerEvent updateControlEvent;
		updateControlEvent.targetLayer = "ICL_Input";
		updateControlEvent.action = InputLayerAction::Update;
		updateControlEvent.layerData = &input;

		auto layerEvent = static_cast<ILayerEvent*>(&layoutCreateEvent);
		auto ile = static_cast<InputLayerEvent*>(layerEvent);
		INDY_CORE_WARN("Action: {0}", (uint8_t)ile->action);

		EventManagerCSR::Notify<ILayerEvent>(layoutCreateEvent);
		EventManagerCSR::Notify<ILayerEvent>(deviceCreateEvent);
		EventManagerCSR::Notify<ILayerEvent>(watchControlEvent);
		EventManagerCSR::Notify<ILayerEvent>(updateControlEvent);
	}

	Sandbox::~Sandbox()
	{
		
	}

	void Sandbox::Run()
	{
		
	}
}