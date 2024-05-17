module;

#include <memory>

export module Indy.Input;

export import :DeviceManager;
export import :DeviceBuilder;
export import :Device;
export import :State;
export import :Control;
export import :Context;

export
{
	namespace Indy
	{
		class Input
		{
		public:
			static std::shared_ptr<InputDeviceManager> GetDeviceManager();

			static void CreateDevice(const InputDeviceInfo& deviceInfo, const InputLayout& deviceLayout);
			static std::shared_ptr<InputDevice> GetDevice(const InputDeviceInfo& deviceInfo);

		private:
			Input() = default;
			~Input() = default;
		};
	}
}