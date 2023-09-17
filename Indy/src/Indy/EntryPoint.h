#pragma once

#ifdef INDY_PLATFORM_WINDOWS

	extern Indy::Application* Indy::CreateApplication();

	int main(int argc, char** argv)
	{
		auto app = Indy::CreateApplication();
		app->Run();
		delete app;
	}

#endif