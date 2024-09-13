module;

#include <memory>
#include <string>

export module Indy.Application;

export {
	namespace Indy
	{
		struct ApplicationCreateInfo
		{
			std::string name;
		};

		struct ApplicationInfo
		{
			std::string name;
		};

		class Application
		{
		public:
			Application(const ApplicationCreateInfo& appInfo);
			virtual ~Application();

			// For use in main (EntryPoint.cpp); Handles the internal main loop
			void StartAndRun();

		private:
			virtual void Load() = 0;
			virtual void Start() = 0;
			virtual void Update() = 0;
			virtual void Shutdown() = 0;
			virtual void Unload() = 0;

		protected:
			ApplicationInfo m_Info;

			// Controls application lifetime. Setting this to true terminates the program
			bool m_ShouldClose = true;

			// Determines whether the application should restart on close.
			bool m_Restart = false;
		};

		// MUST be overridden by CLIENT to define application-specific behavior
		std::unique_ptr<Indy::Application> CreateApplication();

	}
}