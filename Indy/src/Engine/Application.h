#pragma once

#include "Core.h"

namespace Engine
{
	class ENGINE_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// Defined in client.
	Application* CreateApplication();
}



