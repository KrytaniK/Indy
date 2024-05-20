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
			virtual ~InputSystem();

		private:
			virtual void OnLoad();
			virtual void OnUnload();

		private:
			std::unique_ptr<InputDeviceManager> m_DeviceManager;
		};
	}
}