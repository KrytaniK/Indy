#include <Engine/Core/LogMacros.h>

#include <memory>

import Sandbox;

import Indy.Application;
import Indy.Events;
import Indy.Window;
import Indy.Input;
import Indy.VulkanGraphics;

import Indy.Profiler;
import Indy.Graphics;

namespace Indy
{
	Sandbox::Sandbox(const ApplicationCreateInfo& createInfo)
		: Application(createInfo)
	{
		m_ShouldClose = false;
	}

	Sandbox::~Sandbox()
	{
		
	}

	void Sandbox::OnLoad()
	{
		// Load resources from disk
	}

	void Sandbox::OnStart()
	{
		// Post-Load operations

		auto window = Window::Create({ "First",1280, 760, 0 });
		window->GetInputContext()->AddInputCallback(0, 0, [](Input::CallbackEvent* event) { INDY_INFO("Left Mouse Button!"); });
	}

	void Sandbox::OnUpdate()
	{
		// Fixed Update ("Tick")
		// Update
		// Late Update
	}

	void Sandbox::OnUnload()
	{
		// Unload all resources
		// Cleanup phase
	}
}

std::unique_ptr<Indy::Application> CreateApplication()
{
	Indy::ApplicationCreateInfo createInfo;
	createInfo.name = "Sandbox App";

	std::unique_ptr<Indy::Application> app = std::make_unique<Indy::Sandbox>(createInfo);

	return app;
};