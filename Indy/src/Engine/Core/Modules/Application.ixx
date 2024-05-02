module;

#include <memory>

export module Indy_Core:Application;

import :LayerStack;

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

		protected:
			// For use with layered architectures
			std::unique_ptr<LayerStack> m_LayerStack;

			// Controls application lifetime. Setting this to true terminates the program
			bool m_ShouldClose = true;
		};
	}
}