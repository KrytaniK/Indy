import Indy_Core;

extern std::unique_ptr<Indy::Application> Indy::CreateApplication();

int main(int argc, char** argv)
{
	auto app = Indy::CreateApplication();
	app->Run();
}