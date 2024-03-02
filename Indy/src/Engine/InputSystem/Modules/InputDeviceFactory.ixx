module;

#include <functional>
#include <queue>
#include <memory>

export module Indy_Core_InputSystem:DeviceFactory;

import :Device;
import :LayoutMatcher;

export
{
	namespace Indy
	{
		struct InputDeviceFactoryResult
		{
			bool success;
			InputDevice* device;
		};

		class InputDeviceFactory
		{
		private: 
			struct FactoryBlueprint
			{
				std::unique_ptr<InputDeviceInfo> deviceInfo;
				std::function<void(InputDeviceFactoryResult&)> callback;
			};

		public:
			static void Enqueue(InputDeviceInfo* deviceInfo, std::function<void(InputDeviceFactoryResult&)> callback);

		private:
			static void BuildDevice();
			static bool AllocateDeviceState(InputDeviceLayoutInfo* layout, InputDevice* outDevice);
			static bool CreateDeviceControls(InputDeviceLayoutInfo* layout, InputDevice* outDevice);

			static bool s_IsRunning;
			static std::queue<FactoryBlueprint> s_DeviceQueue;
		};
	}
};