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
			static const Application& Get();

		private:
			inline static Application* s_Instance;

		public:
			Application(const ApplicationCreateInfo& appInfo);
			virtual ~Application();

			virtual void OnLoad() = 0;
			virtual void OnUpdate() = 0;
			virtual void OnUnload() = 0;

			// For use in main (EntryPoint.cpp); Handles the internal main loop
			void StartAndRun();

		public:
			EventHandler OnLoad_Event;
			EventHandler OnUpdate_Event;
			EventHandler OnUnload_Event;

		protected:
			ApplicationInfo m_Info;

			// For use with layered architectures
			std::unique_ptr<LayerStack> m_LayerStack;

			// Controls application lifetime. Setting this to true terminates the program
			bool m_ShouldClose = true;

		private:
			std::unique_ptr<InputDeviceManager> m_DeviceManager;
			std::unique_ptr<WindowManager> m_WindowManager;
		};

	}
}

// MUST be overridden by CLIENT to define application-specific behavior
export std::unique_ptr<Indy::Application> CreateApplication();