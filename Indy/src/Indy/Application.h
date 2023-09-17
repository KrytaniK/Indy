#pragma once

#include "Core.h"

namespace Indy
{
	class INDY_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// Defined in client.
	Application* CreateApplication();
}



