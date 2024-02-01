#include <memory>
#include <Engine.h>

import Sandbox;
import Indy_Core;
import Indy_Core_LayerStack;
import Indy_Core_EventSystem;

namespace Indy
{
	std::unique_ptr<Application> CreateApplication()
	{
		return std::make_unique<Sandbox>();
	};

	Sandbox::Sandbox()
	{
		m_Terminate = false;

		OpenWindow({ "Test", 1280, 760 });

		WindowRequestEvent event;
		EventManager::Notify<WindowRequestEvent>(&event);
	}

	Sandbox::~Sandbox()
	{
		
	}

	void Sandbox::Run()
	{
		while (!m_Terminate)
		{
			LayerStack::Update();
		}
	}
}