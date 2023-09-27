#include "SandboxApp.h"

namespace Engine
{
	Application* CreateApplication()
	{
		return new Sandbox();
	}

	Sandbox::Sandbox() {};
	Sandbox::~Sandbox() {};

	void Sandbox::Run()
	{
		INDY_CORE_TRACE("Sandbox Start!");
		while (m_IsRunning)
		{
			m_Window->onUpdate();
		}
		INDY_CORE_TRACE("Sandbox End!");
	}
}