#include <Engine/Core/LogMacros.h>

#include <memory>

import Sandbox;

import Indy.Window;
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
		Graphics::RenderContext* context = Graphics::CreateRenderContext(0, "Scene Example");
		Graphics::SetActiveRenderContext(context);

		if (context)
		{
			// Basic example of deferred rendering
			context->BeginRenderPass("Opaque")
				->EndRenderPass();

			context->BeginRenderPass("Transparent")
				->EndRenderPass();

			context->BeginRenderPass("Post Process")
				->EndRenderPass();
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