#include <memory>

import Sandbox;
import Indy_Core_Profiler;


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
		
	}
}