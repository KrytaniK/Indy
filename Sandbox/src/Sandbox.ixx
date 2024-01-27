module;

#include <Engine.h>

export module Sandbox;

export import <memory>;
export import <iostream>;

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

		private:
			void onAppUpdate(Engine::Event& event);
		};
	}
}