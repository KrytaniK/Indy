#pragma once

#include <Engine.h>

namespace Engine
{
	class Sandbox : public Application
	{
	public:
		Sandbox();
		~Sandbox();

		virtual void Run() override;
	};
}