module;

#include <memory>

export module Sandbox;

import Indy.Application;
import Indy.Input;
import Indy.Window;

export {
	namespace Indy
	{
		class Sandbox : public Application
		{
		public:
			Sandbox();
			virtual ~Sandbox() override;

			virtual void Start() override;
			virtual void Run() override;

		private:
			std::unique_ptr<WindowManager> m_WindowManager;
		};
	}
}