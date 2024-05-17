#include <Engine/Core/LogMacros.h>

#include <memory>
#include <vector>

import Indy.Input;

namespace Indy
{
	InputDeviceManager::InputDeviceManager()
	{
		m_DeviceBuilder = std::make_unique<InputDeviceBuilder>();

		// Reserve enough space up front to refrain from unnecessary reallocations
		m_Devices.reserve(50);
		m_Layouts.reserve(50);
	}

	void InputDeviceManager::AddLayout(const InputLayout& layout)
	{
		// Add the layout
		m_Layouts.emplace_back(layout);

		if (m_DeviceQueue.empty())
			return;

		// If we have any devices that haven't been created, check for
		// a match with the new layout.
		size_t count = 0, limit = m_DeviceQueue.size();
		while (!m_DeviceQueue.empty() && count < limit)
		{
			AddDevice(m_DeviceQueue.front());
			m_DeviceQueue.pop();
			count++;
		}
	}

	void InputDeviceManager::AddDevice(const InputDeviceInfo& deviceInfo)
	{
		// Check against all existing devices.
		for (const auto& device : m_Devices)
		{
			// If specified device class and layout class match an existing device,
			//	odds are the device is the same. No need to build again
			if (device->GetInfo().deviceClass == deviceInfo.deviceClass
				&& device->GetInfo().layoutClass == deviceInfo.layoutClass)
			{
				INDY_CORE_WARN("WARNING: Could not build device [{0}]. Device already exists. Potential Reconnect?", deviceInfo.displayName);

				// In the future, This might set the primary input device,
				//	and/or dispatch events notifying of a device reconnect.
				return;
			}
		}

		// Match device with a device layout
		std::unique_ptr<InputLayout> layout = MatchDeviceLayout(deviceInfo);

		// If no matching layout was found, set aside for when new layouts
		//	are registered.
		if (!layout)
		{
			INDY_CORE_WARN("WARNING: No matching layout was found for device [{0}]", deviceInfo.displayName);
			m_DeviceQueue.push(deviceInfo);
			return;
		}

		INDY_CORE_TRACE("Building Device [{0}]", deviceInfo.displayName);
		// Build and store device
		m_Devices.emplace_back(
			m_DeviceBuilder->Build(deviceInfo, *layout)
		);
	}

	const std::shared_ptr<InputDevice>& InputDeviceManager::GetDevice(const InputDeviceInfo& deviceInfo) const
	{
		for (const auto& device : m_Devices)
		{
			if (device->GetInfo().displayName == deviceInfo.displayName ||
				(device->GetInfo().deviceClass == deviceInfo.deviceClass &&
					device->GetInfo().layoutClass == deviceInfo.layoutClass)
				)
			{
				return device;
			}
		}

		return nullptr;
	}

	void InputDeviceManager::UpdateDeviceState(const InputDeviceInfo& deviceInfo, const std::string& control, std::byte* data)
	{
		if (!data)
		{
			INDY_CORE_ERROR("Could not update device state. Data is null.");
			return;
		}

		std::shared_ptr<InputDevice> device = GetDevice(deviceInfo);

		if (device == nullptr)
		{
			INDY_CORE_ERROR("Could not update device state. No device found.");
			return;
		}

		if (!control.empty())
			device->UpdateControlState(control, data);
		else
			device->UpdateDeviceState(data);
	}

	void InputDeviceManager::WatchDeviceControl(const InputDeviceInfo& deviceInfo, const std::string& control, std::function<void(InputControlContext&)>& onValueChange)
	{

		if (control.empty())
		{
			INDY_CORE_ERROR("Could not watch device control. No control specified.");
			return;
		}

		std::shared_ptr<InputDevice> device = GetDevice(deviceInfo);

		if (device == nullptr)
		{
			INDY_CORE_ERROR("Could not watch device control. No device found.");
			return;
		}

		device->WatchControl(control, onValueChange);
	}

	std::unique_ptr<InputLayout> InputDeviceManager::MatchDeviceLayout(const InputDeviceInfo& deviceInfo)
	{
		float highestPercentMatch = 0.0f;
		float percentMatch = 0.0f;

		int matchIndex = -1;

		// Perform an exhaustive search through all known device layouts.
		int compareIndex = -1;
		for (const InputLayout& layout : m_Layouts)
		{
			++compareIndex;
			percentMatch = 0.0f;

			// Can't match devices to layouts with differing classifications
			if (layout.deviceClass != deviceInfo.deviceClass)
				continue;

			// If device class matches, it is at least a 50% match.
			percentMatch += 50.f;

			// If a layout is specified and matches, this is a 100% match.
			if (layout.layoutClass == deviceInfo.layoutClass)
			{
				percentMatch += 50.f;
				matchIndex = compareIndex;
				break;
			}

			// We've found a match, update values
			if (percentMatch > highestPercentMatch)
			{
				highestPercentMatch = percentMatch;
				matchIndex = compareIndex;
			}
		}

		// If no sufficient match was found, return nullptr
		if (matchIndex < 0 || percentMatch < 75.0f)
			return nullptr;

		// Return a copy of the layout with the highest match percentage
		return std::make_unique<InputLayout>(m_Layouts[matchIndex]);
	}
}