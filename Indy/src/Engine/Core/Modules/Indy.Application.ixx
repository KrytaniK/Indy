module;

#include <memory>
#include <string>
#include <set>

export module Indy.Application;

import Indy.Layers;
import Indy.Events;

import Indy.Input;
import Indy.Window;
import Indy.Graphics;

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
			virtual void Load() = 0;
			virtual void Start() = 0;
			virtual void Update() = 0;
			virtual void Unload() = 0;

		public:
			EventHandler OnLoad;
			EventHandler OnStart;
			EventHandler OnUpdate;
			EventHandler OnUnload;

		protected:
			ApplicationInfo m_Info;

			// Controls application lifetime. Setting this to true terminates the program
			bool m_ShouldClose = true;

			// For use with layered architectures
			std::unique_ptr<LayerStack> m_LayerStack;

			std::unique_ptr<InputSystem> m_InputSystem;
			std::unique_ptr<WindowSystem> m_WindowSystem;
			std::unique_ptr<GraphicsAPI> m_GraphicsAPI;
		};

	}
}

// MUST be overridden by CLIENT to define application-specific behavior
export std::unique_ptr<Indy::Application> CreateApplication();