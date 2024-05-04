#include "Engine/Core/LogMacros.h"

#include <memory>
#include <vector>

import Indy_Core_Input;
import Indy_Core_Events;

namespace Indy
{
	DeviceManager::DeviceManager()
	{
		m_DeviceBuilder = std::make_unique<DeviceBuilder>();

		// Reserve enough space up front to refrain from unnecessary reallocations
		m_Devices.reserve(50);
		m_Layouts.reserve(50);
	}

	void DeviceManager::AddLayout(const DeviceLayout& layout)
	{
		m_Layouts.emplace_back(layout);
	}

	void DeviceManager::AddDevice(const DeviceInfo& deviceInfo)
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
		std::unique_ptr<DeviceLayout> layout = MatchDeviceLayout(deviceInfo);

		// If no matching layout was found, set aside for when new layouts
		//	are registered.
		if (!layout)
		{
			INDY_CORE_WARN("WARNING: No matching layout was found for device [{0}]", deviceInfo.displayName);
			m_DeviceQueue.push(deviceInfo);
			return;
		}

		// Build and store device
		m_Devices.emplace_back(
			m_DeviceBuilder->Build(deviceInfo, *layout)
		);
	}

	std::weak_ptr<Device> DeviceManager::GetDevice(const std::string& displayName)
	{
		for (const auto& device : m_Devices)
		{
			if (device->GetInfo().displayName == displayName)
				return device;
		}

		return std::weak_ptr<Device>();
	}

	std::weak_ptr<Device> DeviceManager::GetDevice(uint16_t deviceClass, uint16_t layoutClass)
	{
		for (const auto& device : m_Devices)
		{
			if (device->GetInfo().deviceClass == deviceClass && device->GetInfo().layoutClass == layoutClass)
			{
				return device;
			}
		}

		return std::weak_ptr<Device>();
	}

	std::weak_ptr<Device> DeviceManager::GetActiveDevice(uint16_t deviceClass)
	{
		for (const auto& device : m_ActiveDevices)
		{
			if (device.first == deviceClass)
				return device.second;
		}

		return std::weak_ptr<Device>();
	}

	void DeviceManager::SetActiveDevice(uint16_t deviceClass, const std::weak_ptr<Device>& activeDevice)
	{
		if (deviceClass != activeDevice.lock()->GetInfo().deviceClass)
			return;

		for (auto& device : m_ActiveDevices)
		{
			if (device.first == deviceClass)
				device.second = activeDevice;
		}
	}

	std::unique_ptr<DeviceLayout> DeviceManager::MatchDeviceLayout(const DeviceInfo& deviceInfo)
	{
		float highestPercentMatch = 0.0f;
		float percentMatch = 0.0f;

		int matchIndex = -1;

		// Perform an exhaustive search through all known device layouts.
		int compareIndex = -1;
		for (const DeviceLayout& layout : m_Layouts)
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
				continue;
			}
		}

		// If no sufficient match was found, return nullptr
		if (matchIndex < 0 || percentMatch < 75.0f)
			return nullptr;

		// Return a copy of the layout with the highest match percentage
		return std::make_unique<DeviceLayout>(m_Layouts[matchIndex]);
	}
}