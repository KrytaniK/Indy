#include <Engine/Core/LogMacros.h>

#include <memory>
#include <iostream>
#include <vector>

#include <GLFW/glfw3.h>

import Sandbox;
import Indy_Core_WindowLayer;
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
	}

	Sandbox::~Sandbox()
	{
		
	}

	void Sandbox::Run()
	{
		
	}
}