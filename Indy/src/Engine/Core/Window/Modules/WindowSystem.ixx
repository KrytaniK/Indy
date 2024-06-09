module;

#include <memory>

export module Indy.Window:System;

import :Manager;
import :Events;

export
{
	namespace Indy
	{
		class WindowSystem
		{
		public:
			WindowSystem();
			~WindowSystem();

		private:
			void OnUpdate();

			void OnWindowCreate(WindowCreateEvent* event);
			void OnWindowDestroy(WindowDestroyEvent* event);
			void OnWindowGet(WindowGetEvent* event);

		private: 
			std::unique_ptr<WindowManager> m_WindowManager;
		};
	}
}