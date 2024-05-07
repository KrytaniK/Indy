#include <Engine/Core/LogMacros.h>

#include <memory>
#include <iostream>
#include <vector>

#include <GLFW/glfw3.h>

import Sandbox;
import Indy_Core_WindowLayer;
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

		auto vk_Renderer = VulkanRenderer();
	}

	Sandbox::~Sandbox()
	{
		
	}

	void Sandbox::Run()
	{
		
	}
}