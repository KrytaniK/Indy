#include <Engine/Core/LogMacros.h>

#include <vector>
#include <memory>
#include <functional>
#include <string>

import Indy.Input;

namespace Indy
{
	InputContext::InputContext(const std::shared_ptr<InputDeviceManager>& deviceManager, const std::vector<InputDeviceInfo>& deviceInfos)
		: m_DeviceManager(deviceManager)
	{
		// Copy desired input devices
		for (const auto& deviceInfo : deviceInfos)
		{
			std::shared_ptr<InputDevice> device = m_DeviceManager->GetDevice(deviceInfo);

			if (device == nullptr)
				continue;

			m_Devices.emplace(std::make_pair(device->GetInfo().displayName, *device.get()));
		}
	}

	InputContext::InputContext(const std::shared_ptr<InputDeviceManager>& deviceManager, const std::vector<std::string>& deviceNames)
		: m_DeviceManager(deviceManager)
	{
		// Copy desired input devices
		for (const auto& name : deviceNames)
		{
			InputDeviceInfo deviceInfo;
			deviceInfo.displayName = name;

			std::shared_ptr<InputDevice> device = m_DeviceManager->GetDevice(deviceInfo);

			if (device == nullptr)
				continue;

			m_Devices.emplace(std::make_pair(device->GetInfo().displayName, *device.get()));
		}
	}

	void InputContext::OnValueChange(const ::std::string& deviceName, const std::string& controlName,
		std::function<void(InputControlContext&)> callback)
	{
		InputDevice* device = GetDevice(deviceName);

		if (!device)
		{
			INDY_CORE_ERROR("Could not bind callback for [{0}][{1}]. Could not find device.", deviceName, controlName);
			return;
		}

		device->WatchControl(controlName, callback);
	}

	void InputContext::UpdateDevice(const std::string& deviceName, void* newState)
	{
		InputDevice* device = GetDevice(deviceName);

		if (!device)
		{
			INDY_CORE_ERROR("Could not update state for [{0}]. Could not find device.", deviceName);
			return;
		}

		device->UpdateDeviceState(static_cast<std::byte*>(newState));
	}

	void InputContext::UpdateControl(const std::string& deviceName, const std::string& controlName, void* newState)
	{
		InputDevice* device = GetDevice(deviceName);

		if (!device)
		{
			INDY_CORE_ERROR("Could not update control state for [{0}][{1}]. Could not find device.", deviceName, controlName);
			return;
		}

		device->UpdateControlState(controlName, static_cast<std::byte*>(newState));
	}

	InputDevice* InputContext::GetDevice(const std::string& deviceName)
	{
		auto iterator = m_Devices.find(deviceName);

		if (iterator == m_Devices.end())
			return nullptr;

		return &iterator->second;
	}
}
