#include "LogMacros.h"

#include "Engine/LayerStack/LayerStack.h"
#include "Engine/Platform/RendererAPI/RenderContext.h"
#include "Engine/LayerStack/WindowLayer/WindowLayer.h"
#include "Engine/Renderer/Renderer.h"

import Indy_Core;

extern std::unique_ptr<Indy::Application> Indy::CreateApplication();

int main(int argc, char** argv)
{
	auto app = Indy::CreateApplication();
	app->Run();
}

namespace Indy
{
	//using namespace Engine;

	Application::Application()
	{
		Engine::RenderContext::Set(RENDER_API_VULKAN);
		Engine::LayerStack::Push(new Engine::WindowLayer());
		Engine::Renderer::Init();
	}

	Application::~Application()
	{
		
	}

	// Base Implementation
	void Application::Run()
	{
		INDY_CORE_INFO("Uh oh! You need to override Application::Run()! Press Enter or Esc to exit.");

		std::cin.get();

		INDY_CORE_INFO("Goodbye!");
	} 

	void Application::Terminate()
	{
		this->m_Terminate = true;
	}
}