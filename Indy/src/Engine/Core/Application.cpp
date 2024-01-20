#include "Log.h"
#include "Engine/LayerStack/LayerStack.h"
#include "Engine/Platform/RendererAPI/RenderContext.h"
#include "Engine/LayerStack/WindowLayer/WindowLayer.h"
#include "Engine/Renderer/Renderer.h"

import Core;

extern std::unique_ptr<Indy::Application> Indy::CreateApplication();

int main(int argc, char** argv)
{
	auto app = Indy::CreateApplication();
	app->Run();
}

namespace Indy
{
	using namespace Engine;

	Application::Application()
	{
		std::cout << "Application Constructor" << std::endl;

		Log::Init();
		RenderContext::Set(RENDER_API_VULKAN);
		LayerStack::Push(new WindowLayer());
		Renderer::Init();
	}

	Application::~Application()
	{
		std::cout << "Application Destructor" << std::endl;
	}

	// Base Implementation
	void Application::Run()
	{
		std::cout << "Application Run START!" << std::endl;

		std::cin.get();

		std::cout << "Application Run END!" << std::endl;
	} 

	void Application::Terminate()
	{
		this->m_Terminate = true;
	}
}