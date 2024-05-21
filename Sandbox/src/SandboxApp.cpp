#include <Engine/Core/LogMacros.h>

#include <memory>

import Sandbox;

import Indy.Application;
import Indy.Events;
import Indy.Window;
import Indy.Input;

namespace Indy
{
	struct Vec2D
	{
		double x, y;
	};

	Sandbox::Sandbox(const ApplicationCreateInfo& createInfo)
		: Application(createInfo)
	{
		m_ShouldClose = false;

		WindowCreateInfo firstInfo = { "First", 1280, 760, 0 };

		WindowCreateEvent event;
		event.createInfo = &firstInfo;

		Events<WindowCreateEvent>::Notify(&event);
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

		m_InputContext.AddInputCallback(
			0,
			0,
			[](Input::CallbackEvent* event)
			{
				INDY_INFO("{0}", event->context->ReadAs<bool>());
			}
		);
		
		m_InputContext.AddInputCallback(
			0,
			8,
			[](Input::CallbackEvent* event)
			{
				INDY_INFO("x: {0}, y: {1}", event->context->ReadAs<Vec2D>().x, event->context->ReadAs<Vec2D>().y);
			}
		);

		Input::SetContextEvent event;
		event.newContext = &m_InputContext;

		Events<Input::SetContextEvent>::Notify(&event);
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