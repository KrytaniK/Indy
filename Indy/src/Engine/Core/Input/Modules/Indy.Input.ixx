module;

export module Indy.Input;

export import :DeviceManager;
export import :DeviceBuilder;
export import :Device;
export import :State;
export import :Control;
export import :System;
export import :Events;

export
{
	namespace Indy::Input
	{
		// Maybe these attach to a "global" InputContext

		//// Targets default keyboard
		//static void OnKeyPress(const std::string& key, std::function<void(InputControlContext&)> callback);
		//static void OnKeyHold(const std::string& key, std::function<void(InputControlContext&)> callback);
		//static void OnKeyRelease(const std::string& key, std::function<void(InputControlContext&)> callback);

		//// Targets default game pad
		//static void OnButtonPress(const std::string& button, std::function<void(InputControlContext&)> callback);
		//static void OnButtonHold(const std::string& button, std::function<void(InputControlContext&)> callback);
		//static void OnButtonRelease(const std::string& button, std::function<void(InputControlContext&)> callback);
		//static void OnJoystickPress(const std::string& joystick, std::function<void(InputControlContext&)> callback);
		//static void OnJoystickRelease(const std::string& joystick, std::function<void(InputControlContext&)> callback);
		//static void OnTriggerPull(const std::string& trigger, std::function<void(InputControlContext&)> callback);
		//static void OnTriggerRelease(const std::string& trigger, std::function<void(InputControlContext&)> callback);

		//// Targets default mouse
		//static void OnMouseButtonPress(const std::string& mouseButton, std::function<void(InputControlContext&)> callback);
		//static void OnMouseButtonRelease(const std::string& mouseButton, std::function<void(InputControlContext&)> callback);
	}
}