#include <memory>

import Indy_Core;

// CreateApplication is defined by CLIENT applications.
extern std::unique_ptr<Indy::Application> Indy::CreateApplication();

int main(int argc, char** argv)
{
	auto app = Indy::CreateApplication();
	app->StartAndRun();
}