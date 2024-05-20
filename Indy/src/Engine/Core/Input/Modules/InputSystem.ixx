module;

#include <memory>

export module Indy.Input:System;

import :DeviceManager;
import :Events;

export
{
	namespace Indy
	{
		class InputSystem
		{
		public:
			InputSystem();
			~InputSystem() = default;

		private:
			void OnLoad();
			void OnUnload();

			void OnInput(Input::Event* event);

		private:
			std::unique_ptr<Input::DeviceManager> m_DeviceManager;
		};
	}
}