#include <Engine/Core/ProfileMacros.h>

#include <memory>
#include <typeindex>
#include <Engine.h>
#include <iostream>

import Sandbox;
import Indy_Core;
import Indy_Core_LayerStack;
import Indy_Core_InputSystem;
import Indy_Core_EventSystem;

namespace Indy
{
	std::unique_ptr<Application> CreateApplication()
	{
		return std::make_unique<Sandbox>();
	};

	Sandbox::Sandbox()
	{
		m_Terminate = false;

		OpenWindow({"Test Window", 1280, 760});

		InputDeviceLayoutInfo keyboardLayoutInfo;
		keyboardLayoutInfo.classification = "Keyboard";
		keyboardLayoutInfo.vendorID = "Default";
		keyboardLayoutInfo.productID = "Keyboard";
		keyboardLayoutInfo.format = 1;

		InputDeviceControlInfo keySpace;
		keySpace.displayName = "Space";
		keySpace.layout = "Button";
		keySpace.offset = 0;
		keySpace.bit = 0;
		keySpace.sizeInBits = 1;

		keyboardLayoutInfo.controls.emplace_back(keySpace);

		InputDeviceLayoutInfo layoutInfo;
		layoutInfo.classification = "Pointer";
		layoutInfo.vendorID = "Default";
		layoutInfo.productID = "Mouse";
		layoutInfo.format = 0;

		InputDeviceControlInfo lmb;
		lmb.displayName = "Left Mouse Button";
		lmb.layout = "Button";
		lmb.offset = 0;
		lmb.bit = 0;
		lmb.sizeInBits = 1;

		InputDeviceControlInfo rmb;
		rmb.displayName = "Right Mouse Button";
		rmb.layout = "Button";
		rmb.offset = 0;
		rmb.bit = 1;
		rmb.sizeInBits = 1;

		InputDeviceControlInfo position;
		position.displayName = "Position";
		position.layout = "Axis";
		position.format = "Vec2";
		position.offset = 1;
		position.sizeInBits = (sizeof(float) * 2) * 8;

		layoutInfo.controls.emplace_back(lmb);
		layoutInfo.controls.emplace_back(rmb);
		layoutInfo.controls.emplace_back(position);

		InputManager::RegisterLayout(layoutInfo);
		InputManager::RegisterLayout(keyboardLayoutInfo);

		InputDeviceDetectEvent event;
		event.deviceInfo.vendorID = "Default";
		event.deviceInfo.productID = "Mouse";
		event.deviceInfo.classification = "Pointer";
		event.deviceInfo.format = 0;
		event.deviceInfo.displayName = "Default Mouse";

		EventManager::Notify<InputDeviceDetectEvent>(&event);

		event.deviceInfo.vendorID = "Default";
		event.deviceInfo.productID = "Keyboard";
		event.deviceInfo.classification = "Keyboard";
		event.deviceInfo.format = 1;
		event.deviceInfo.displayName = "Default Keyboard";

		EventManager::Notify<InputDeviceDetectEvent>(&event);
	}

	Sandbox::~Sandbox()
	{
		
	}

	void Sandbox::Run()
	{
		const InputDeviceControlInfo& lmb = InputManager::GetDeviceControlInfo(0, "Left Mouse Button");
		const InputDeviceControlInfo& rmb = InputManager::GetDeviceControlInfo(0, "Right Mouse Button");
		InputManager::WatchDeviceControl(lmb, [=](const InputDeviceControlValue& value) { INDY_CORE_INFO("LMB Clicked? {0}", value.ReadAs<bool>()); });
		InputManager::WatchDeviceControl(rmb, [=](const InputDeviceControlValue& value) { INDY_CORE_INFO("RMB Clicked? {0}", value.ReadAs<bool>()); });

		const InputDeviceControlInfo& space = InputManager::GetDeviceControlInfo(1, "Space");
		InputManager::WatchDeviceControl(space, [=](const InputDeviceControlValue& value) { INDY_CORE_INFO("Space Pressed? {0}", value.ReadAs<bool>()); });

		// Primary Loop
		while (!m_Terminate)
		{
			LayerStack::Update();
		}
	}
}