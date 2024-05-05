#include <Engine/Core/LogMacros.h>

#include <memory>
#include <iostream>
#include <vector>

#include <GLFW/glfw3.h>

import Sandbox;
import Indy_Core;
import Indy_Core_InputLayer;
import Indy_Core_WindowLayer;
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
		m_ShouldClose = false;

		m_LayerStack->PushLayer(std::make_shared<WindowLayer>());
		m_LayerStack->PushLayer(std::make_shared<InputLayer>());

		AD_WindowCreateInfo windowCreateInfo;
		windowCreateInfo.title = "Test Window";
		windowCreateInfo.width = 1200;
		windowCreateInfo.height = 760;
		windowCreateInfo.id = 0;

		WindowLayerEvent windowCreateEvent;
		windowCreateEvent.targetLayer = "ICL_Window";
		windowCreateEvent.action = WindowLayerAction::OpenWindow;
		windowCreateEvent.layerData = &windowCreateInfo;

		EventManagerCSR::Notify<ILayerEvent>(&windowCreateEvent);

		AD_InputWatchControlInfo watchControlInfo;
		watchControlInfo.deviceClass = 0x0001;
		watchControlInfo.layoutClass = 0x4B42;
		watchControlInfo.control = std::to_string(GLFW_KEY_A);
		watchControlInfo.callback = [=](DeviceControlContext& ctx)
			{
				INDY_CORE_INFO(
					"Pressed? {0}",
					ctx.ReadAs<bool>()
				);
			};

		InputLayerEvent watchEvent;
		watchEvent.targetLayer = "ICL_Input";
		watchEvent.action = InputLayerAction::WatchControl;
		watchEvent.layerData = &watchControlInfo;

		EventManagerCSR::Notify<ILayerEvent>(&watchEvent);
	}

	Sandbox::~Sandbox()
	{
		
	}

	void Sandbox::Run()
	{
		
	}
}