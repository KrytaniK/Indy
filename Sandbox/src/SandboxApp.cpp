#include <Engine/Core/LogMacros.h>

#include <memory>
#include <iostream>
#include <vector>

#include <GLFW/glfw3.h>

import Sandbox;
import Indy_Core_WindowLayer;
import Indy_Core_InputLayer;
import Indy_Core_Renderer;

namespace Indy
{
	std::unique_ptr<Application> CreateApplication()
	{
		return std::make_unique<Sandbox>();
	};

	Sandbox::Sandbox()
	{
		m_ShouldClose = false;

		m_LayerStack->PushLayer(std::make_shared<WindowLayer>());
		m_LayerStack->PushLayer(std::make_shared<InputLayer>());

		auto renderer = VulkanRenderer();

		DeviceInfo device;
		device.deviceClass = 0x0000; // "Pointer"
		device.layoutClass = 0x4D53; // "GLFW Mouse"

		ICL_InputData_WatchControl watchData;
		watchData.targetDevice = &device;
		watchData.targetControl = "Mouse Scroll";
		watchData.callback = [](DeviceControlContext& ctx)
			{
				INDY_CORE_INFO("Scroll!");
			};

		ICL_InputEvent inputEvent;
		inputEvent.targetLayer = "ICL_Input";
		inputEvent.action = ICL_Action::WatchControl;
		inputEvent.layerData = &watchData;

		EventManagerCSR::Notify<ILayerEvent>(&inputEvent);
	}

	Sandbox::~Sandbox()
	{
		
	}

	void Sandbox::Run()
	{
		
	}
}