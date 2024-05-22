module;

#include <memory>
#include <string>
#include <set>

export module Indy.Application;

import Indy.Layers;
import Indy.Events;
import Indy.Input;
import Indy.Window;

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
			static Application& Get();

		private:
			static Application* s_Instance;

		public:
			Application(const ApplicationCreateInfo& appInfo);
			virtual ~Application();

			// For use in main (EntryPoint.cpp); Handles the internal main loop
			void StartAndRun();

		private:
			virtual void OnLoad() = 0;
			virtual void OnStart() = 0;
			virtual void OnUpdate() = 0;
			virtual void OnUnload() = 0;

		public:
			EventHandler Load;
			EventHandler OnStart_Event;
			EventHandler Update;
			EventHandler Unload;

		protected:
			ApplicationInfo m_Info;

			// For use with layered architectures
			std::unique_ptr<LayerStack> m_LayerStack;

			// Controls application lifetime. Setting this to true terminates the program
			bool m_ShouldClose = true;

		private:
			std::unique_ptr<InputSystem> m_InputSystem;
			std::unique_ptr<WindowSystem> m_WindowSystem;
		};

	}
}

// MUST be overridden by CLIENT to define application-specific behavior
export std::unique_ptr<Indy::Application> CreateApplication();