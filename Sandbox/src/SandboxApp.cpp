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
		Graphics::Context& context = Graphics::CreateContext("Scene Example");
		Graphics::SetActiveContext(context.GetID());

		context.AddRenderPass("Main").Begin()
			.AddProcess(Indy::Graphics::RenderPass::MainProcess)
			.BindShader(Indy::Graphics::PIPELINE_SHADER_STAGE_VERTEX, "shaders/vertex.glsl.vert")
			.BindShader(Indy::Graphics::PIPELINE_SHADER_STAGE_FRAGMENT, "shaders/fragment.glsl.frag")
			.AddProcess(Indy::Graphics::RenderPass::PreProcess)
			.BindShader(Indy::Graphics::PIPELINE_SHADER_STAGE_COMPUTE, "shaders/compute.glsl.comp")
			.End();
	}

	void Sandbox::Update()
	{
		
	}

	void Sandbox::Shutdown()
	{
		Graphics::Shutdown();
	}

	void Sandbox::Unload()
	{
		
	}
}