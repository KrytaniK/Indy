export module Indy_Core:Application;

import Indy_Core_EventSystem;

export {
	namespace Indy
	{
		class Application
		{
		public:
			Application();
			virtual ~Application();
			virtual void Run() = 0;

			void Terminate();

		private:
			void onEvent(ApplicationEvent* event);

		protected:
			bool m_Terminate;
		};
	}
}