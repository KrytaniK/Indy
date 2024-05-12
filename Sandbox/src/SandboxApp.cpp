
#include <memory>

import Sandbox;

import Indy.Application;
import Indy.Events;
import Indy.Input;

std::unique_ptr<Indy::Application> CreateApplication()
{
	std::unique_ptr<Indy::Application> app = std::make_unique<Indy::Sandbox>();

	return app;
};

namespace Indy
{
	Sandbox::Sandbox()
	{
		m_ShouldClose = true;
	}

	Sandbox::~Sandbox()
	{
		
	}

	void Sandbox::Run()
	{
		
	}
}