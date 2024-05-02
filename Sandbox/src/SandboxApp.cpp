#include <Engine/Core/LogMacros.h>

#include <memory>
#include <iostream>

import Sandbox;
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
		DeviceLayout myDeviceLayout;
		myDeviceLayout.deviceClass = 0x0000;
		myDeviceLayout.layoutClass = 0x0000;
		myDeviceLayout.sizeInBytes = 0x0001;
		myDeviceLayout.controls = {
			{"Test 1", 0, 0, 0, 0, 2},
			{"1 Left Button", 0, 1, 0, 1, 0},
			{"1 Right Button", 0, 1, 0, 2, 0},
			{"Test 2", 0, 0, 0, 0, 2},
			{"2 Left Button", 0, 1, 0, 3, 0},
			{"2 Right Button", 0, 1, 0, 4, 0},
		};

		DeviceManager dvMng;
		dvMng.AddLayout(myDeviceLayout);

		// Device Detection
		DeviceInfo device;
		device.displayName = "My Device";
		device.deviceClass = 0x0000;
		device.layoutClass = 0x0000;

		DeviceDetectEvent ddEvent;
		ddEvent.deviceInfo = device;

		EventManagerCSR::Notify(ddEvent);

		std::weak_ptr<Device> myDevice = dvMng.GetDevice(0x0000, 0x0000);
		std::weak_ptr<Device> myActiveDevice = dvMng.GetActiveDevice(0x0000);

		auto leftBtn = myDevice.lock()->GetControl("1 Left Button");

		if (leftBtn.expired())
			return;

		
	}

	Sandbox::~Sandbox()
	{
		
	}

	void Sandbox::Run()
	{
		
	}
}