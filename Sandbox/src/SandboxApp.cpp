#include <Engine/Core/LogMacros.h>

#include <memory>
#include <iostream>
#include <vector>

import Sandbox;
import Indy_Core_WindowLayer;
import Indy_Core_InputLayer;
import Indy_Core_Renderer;

namespace Indy
{
	struct MyEvent 
	{
		int x;
	};

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
	}

	Sandbox::~Sandbox()
	{
		
	}

	void Sandbox::Run()
	{
		
	}
}