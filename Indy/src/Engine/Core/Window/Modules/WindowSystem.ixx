module;

#include <memory>

export module Indy.Window:System;

import :Manager;
import :Events;

import Indy.Graphics;

export
{
	namespace Indy
	{
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
			std::unique_ptr<Graphics::IRenderAPI> m_RenderAPI;
			std::unique_ptr<WindowManager> m_WindowManager;
		};
	}
}