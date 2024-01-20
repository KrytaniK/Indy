#pragma once

#include <Engine.h>

import Core;

// Class Implementation
namespace Indy
{
	class Sandbox : public Application
	{
	public:
		Sandbox();
		~Sandbox();

		virtual void Run() override;

	private:
		void onAppUpdate(Engine::Event& event);
	};
}