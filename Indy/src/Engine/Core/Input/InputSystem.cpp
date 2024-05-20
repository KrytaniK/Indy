#include <memory>

import Indy.Application;
import Indy.Input;
import Indy.Events;

namespace Indy
{
	using namespace Input;

	InputSystem::InputSystem()
	{
		m_DeviceManager = std::make_unique<InputDeviceManager>();

		// Sync Application Events
		Application& app = Application::Get();
		app.OnLoad_Event.Subscribe<InputSystem>(this, &InputSystem::OnLoad);
		app.OnUnload_Event.Subscribe<InputSystem>(this, &InputSystem::OnUnload);

		// Bind Event Handles
	}

	InputSystem::~InputSystem()
	{
		
	}

	void InputSystem::OnLoad()
	{
		
	}

	void InputSystem::OnUnload()
	{
		
	}
}
