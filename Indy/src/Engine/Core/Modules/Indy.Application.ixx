module;

#include <memory>
#include <string>

export module Indy.Application;

import Indy.Layers;

export {
	namespace Indy
	{
		class Application
		{
		public:
			Application();
			virtual ~Application();

			// For use in main (EntryPoint.cpp); Handles the internal main loop
			void StartAndRun();

			// Application startup method. Called before the main loop
			virtual void Start() = 0;

			// Application's main loop.
			virtual void Run() = 0;

			// Helper functions for layer operations via CreateApplication
			void PushLayer(const std::shared_ptr<ILayer>& layer);
			template<typename T>
			void PushLayer();
			void PushOverlay(const std::shared_ptr<ILayer>& overlay);
			template<typename T>
			void PushOverlay();

		protected:
			// For use with layered architectures
			std::unique_ptr<LayerStack> m_LayerStack;

			// Controls application lifetime. Setting this to true terminates the program
			bool m_ShouldClose = true;
		};

		// Template Definitions

		template<typename T>
		void Application::PushLayer()
		{
			static_assert(std::is_base_of_v<ILayer, T>);

			m_LayerStack->PushLayer(std::make_shared<T>());
		}

		template<typename T>
		void Application::PushOverlay()
		{
			static_assert(std::is_base_of_v<ILayer, T>);

			m_LayerStack->PushOverlay(std::make_shared<T>());
		}

	}
}

// MUST be overridden by CLIENT to define application-specific behavior
export std::unique_ptr<Indy::Application> CreateApplication();