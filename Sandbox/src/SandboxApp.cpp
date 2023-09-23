#include <Engine.h>

namespace Engine
{
	class Sandbox : public Application
	{
	public:
		Sandbox() {};

		~Sandbox() {};

		virtual void Run() override;
	};

	void Sandbox::Run()
	{
		INDY_CORE_TRACE("Sandbox Start!");
	}

	Application* CreateApplication()
	{
		return new Sandbox();
	}
}