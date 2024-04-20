module;

#include <memory>

export module Indy_Core:Application;

import Indy_Core_EventSystem;
import Indy_Core_LayerStack;

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
			std::unique_ptr<LayerStack> m_LayerStack;
			bool m_ShouldClose = false;
		};
	}
}