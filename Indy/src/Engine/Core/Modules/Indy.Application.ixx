module;

#include <memory>

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

			// Automatically gets called repeatedly until m_ShouldClose is true
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
	}
}

// MUST be overridden by CLIENT to define application-specific behavior
export std::unique_ptr<Indy::Application> CreateApplication();