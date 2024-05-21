module;

#include <memory>

export module Indy.Input:System;

import :DeviceManager;
import :Events;
import :Context;

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
			void OnGetDevice(Input::DeviceGetEvent* event);
			void OnSetContext(Input::SetContextEvent* event);

		private:
			Input::InputContext* m_ActiveContext = nullptr;
			std::unique_ptr<Input::DeviceManager> m_DeviceManager;
		};
	}
}