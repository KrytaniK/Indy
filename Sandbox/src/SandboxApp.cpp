#include <Engine/Core/LogMacros.h>

#include <memory>

import Sandbox;

import Indy.Graphics;

namespace Indy
{
	std::unique_ptr<Indy::Application> CreateApplication()
	{
		Indy::ApplicationCreateInfo createInfo;
		createInfo.name = "Sandbox App";

		return std::make_unique<Indy::Sandbox>(createInfo);
	};

	Sandbox::Sandbox(const ApplicationCreateInfo& createInfo)
		: Application(createInfo)
	{
		m_ShouldClose = true;
	}

	Sandbox::~Sandbox()
	{
		
	}

	void Sandbox::Load()
	{
		Graphics::Init(Graphics::Driver::Vulkan);
	}

	void Sandbox::Start()
	{
		Graphics::RenderContext* context = Graphics::AddRenderContext(0);

		if (context)
		{
			context->AddRenderPass("Main Pass");
			// Configuration Stuff (Chaining is possible)
			context->EndRenderPass();
		}
	}

	void Sandbox::Update()
	{
		
	}

	void Sandbox::Shutdown()
	{
		
	}

	void Sandbox::Unload()
	{
		
	}
}