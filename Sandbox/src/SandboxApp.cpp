#include <Engine/Core/ProfileMacros.h>

#include <Engine.h>

import Sandbox;
import Indy_Core;
import Indy_Core_LayerStack;
import Indy_Core_InputSystem;
import Indy_Core_EventSystem;

namespace Indy
{
	std::unique_ptr<Application> CreateApplication()
	{
		return std::make_unique<Sandbox>();
	};

	Sandbox::Sandbox()
	{
		
	}

	Sandbox::~Sandbox()
	{
		
	}

	void Sandbox::Run()
	{
		INDY_INFO("Sandbox::Run()");
	}
}