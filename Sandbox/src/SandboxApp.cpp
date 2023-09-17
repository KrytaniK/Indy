#include <Indy.h>

class Sandbox : public Indy::Application
{
public:
	Sandbox() {};
	~Sandbox() {};
};

Indy::Application* Indy::CreateApplication()
{
	return new Sandbox();
}