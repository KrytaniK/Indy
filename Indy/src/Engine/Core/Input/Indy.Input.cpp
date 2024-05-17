#include <memory>

import Indy.Input;

namespace Indy
{
	std::shared_ptr<InputDeviceManager> Input::GetDeviceManager()
	{
		static std::shared_ptr<InputDeviceManager> s_DeviceManager = std::make_shared<InputDeviceManager>();
		return s_DeviceManager;
	}

	void Input::CreateDevice(const InputDeviceInfo& deviceInfo, const InputLayout& deviceLayout)
	{
		std::shared_ptr<InputDeviceManager> deviceManager = Input::GetDeviceManager();

		deviceManager->AddLayout(deviceLayout);
		deviceManager->AddDevice(deviceInfo);
	}

	std::shared_ptr<InputDevice> Input::GetDevice(const InputDeviceInfo& deviceInfo)
	{
		return Input::GetDeviceManager()->GetDevice(deviceInfo);
	}
}
