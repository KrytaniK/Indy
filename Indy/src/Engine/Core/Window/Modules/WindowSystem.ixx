module;

#include <memory>

export module Indy.Window:System;

import :Manager;
import :Events;

export
{
	namespace Indy
	{
		class GraphicsAPI;

		class WindowSystem
		{
		public:
			WindowSystem();
			virtual ~WindowSystem();

		private:
			virtual void OnLoad();
			virtual void OnUpdate();
			virtual void OnUnload();

			virtual void OnWindowCreate(WindowCreateEvent* event);
			virtual void OnWindowDestroy(WindowDestroyEvent* event);
			virtual void OnWindowGet(WindowGetEvent* event);

		private:
			std::unique_ptr<GraphicsAPI> m_GraphicsAPI;
			std::unique_ptr<WindowManager> m_WindowManager;
		};
	}
}