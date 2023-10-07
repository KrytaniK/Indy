#include "SandboxApp.h"

namespace Engine
{
	Application* CreateApplication()
	{
		return new Sandbox();
	}

	Sandbox::Sandbox() {};
	Sandbox::~Sandbox() {};

}