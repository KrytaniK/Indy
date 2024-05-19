#include <Engine/Core/LogMacros.h>

#include <memory>

import Sandbox;

import Indy.Application;

namespace Indy
{
	Sandbox::Sandbox(const ApplicationCreateInfo& createInfo)
		: Application(createInfo)
	{
		m_ShouldClose = true;
	}

	Sandbox::~Sandbox()
	{
		
	}

	void Sandbox::OnLoad()
	{
		// Load resources from disk
	}

	void Sandbox::OnUpdate()
	{
		// Fixed Update ("Tick")
		// Update
		// Late Update
		// Render
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
	createInfo.name = "Sandbox";
	createInfo.features = {
		"Input",
		"Window"
	};

	std::unique_ptr<Indy::Application> app = std::make_unique<Indy::Sandbox>(createInfo);

	return app;
};