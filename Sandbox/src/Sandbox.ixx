module;

export module Sandbox;

import Indy_Core;

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