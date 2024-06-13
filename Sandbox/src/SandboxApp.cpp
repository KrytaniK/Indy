#include <Engine/Core/LogMacros.h>

#include <memory>

import Sandbox;

import Indy.Application;
import Indy.Events;
import Indy.Window;

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

	void Sandbox::Load()
	{
		// Load resources from disk

		// Initialize Graphics API (Only Vulkan is currently supported)
		m_GraphicsAPI = GraphicsAPI::Create(GraphicsAPI::Vulkan);
	}

	void Sandbox::Start()
	{
		// Post-Load operations

		// Create the main window
		WindowCreateInfo createInfo{ "Sandbox - Indy's Testing Ground", 1280, 768, 0 };
		Window::Create(createInfo);
	}

	void Sandbox::Update()
	{
		// Fixed Update ("Tick")
		// Update
		// Late Update
	}

	void Sandbox::Unload()
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