module;

export module Sandbox;

import Indy_Core;
import Indy_Core_InputSystem;

export {
	namespace Indy
	{
		class Sandbox : public Application
		{
		public:
			Sandbox();
			~Sandbox();

			virtual void Run() override;
		};
	}
}