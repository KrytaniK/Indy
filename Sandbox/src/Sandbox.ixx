module;

#include <Engine.h>

export module Sandbox;

export import <memory>;

import Indy_Core;
import EventSystem;

export {
	namespace Indy
	{
		class Sandbox : public Application
		{
		public:
			Sandbox();
			~Sandbox();

			void OnWindowCreate(WindowCreateEvent* event);

			virtual void Run() override;

		private:
			void onAppUpdate(Engine::Event& event);
		};
	}
}