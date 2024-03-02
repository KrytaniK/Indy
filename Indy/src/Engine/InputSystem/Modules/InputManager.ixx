module;

#include <queue>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <functional>

export module Indy_Core_InputSystem:InputManager;

import Indy_Core_EventSystem;

import :Device;
import :DeviceControl;
import :Events;
import :LayoutMatcher;

export 
{
	namespace Indy
	{
		class InputManager
		{
		public:
			InputManager();
			~InputManager();

			static void RegisterLayout(const InputDeviceLayoutInfo& layout);

			static const InputDeviceControlInfo& GetDeviceControlInfo(int deviceFormat, const std::string& control);
			static const InputDeviceControlInfo& GetDeviceControlInfo(const std::string& deviceName, const std::string& control);
			
			static void WatchDeviceControl(const InputDeviceControlInfo& controlInfo, const std::function<void(const InputDeviceControlValue&)>& onValueChange);

			template<class C>
			static void WatchDeviceControl(const InputDeviceControlInfo& controlInfo, C* instance, void(C::* callback)(const InputDeviceControlValue&))
			{
				InputManager::WatchDeviceControl(controlInfo, [=](const InputDeviceControlValue& value) { (instance->*callback)(value); });
			}

			void ProcessEvents();

			void Enable();
			void Disable();
			bool Enabled() const { return m_IsEnabled; };


		private:
			// Event Handlers
			void OnDeviceDetected(InputDeviceDetectEvent* event); // Push event to queue
			void OnDeviceEvent(InputDeviceEvent* event);  // Push event to queue

			std::vector<IEventHandle> m_EventHandles; // Container for event handles
			std::queue<std::unique_ptr<InputDeviceEvent>> m_DeviceEventQueue; // Device Event Queue
			bool m_IsEnabled;

			static std::map<int, InputDevice*> s_Devices;
		};
	}
}