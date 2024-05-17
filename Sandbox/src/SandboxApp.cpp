#include <Engine/Core/LogMacros.h>

#include <memory>
#include <string>
#include <vector>

import Sandbox;

import Indy.Application;
import Indy.Window;
import Indy.Input;
import Indy.Renderer;

std::unique_ptr<Indy::Application> CreateApplication()
{
	std::unique_ptr<Indy::Application> app = std::make_unique<Indy::Sandbox>();

	return app;
};

namespace Indy
{
	Sandbox::Sandbox()
	{
		m_ShouldClose = false;
	}

	Sandbox::~Sandbox()
	{
		
	}

	void Sandbox::Start()
	{
		INDY_INFO("Sandbox Start");

		m_WindowManager = std::make_unique<WindowManager>();

		WindowCreateInfo primary{ "Primary Window", 1260, 720, RenderAPI::Vulkan, 0 };
		WindowCreateInfo secondary{ "Secondary Window", 1260, 720, RenderAPI::Vulkan, 1 };
		WindowCreateInfo tertiary{ "Tertiary Window", 1260, 720, RenderAPI::Vulkan, 2 };

		m_WindowManager->AddWindow(primary);
		m_WindowManager->AddWindow(secondary);
		m_WindowManager->AddWindow(tertiary);
	}

	void Sandbox::Run()
	{
		m_WindowManager->Update();
	}
}