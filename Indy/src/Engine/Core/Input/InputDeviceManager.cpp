#include <Engine/Core/LogMacros.h>

#include <memory>
#include <unordered_map>

import Indy.Input;

namespace Indy::Input
{
	DeviceManager::DeviceManager()
	{
		m_DeviceBuilder = std::make_unique<DeviceBuilder>();
	}

	void DeviceManager::AddLayout(const Layout& layout)
	{
		if (m_LayoutMap[layout.deviceClass].contains(layout.id))
		{
			INDY_CORE_ERROR("Could not register Input Device Layout with classification [{0}]. Layout already exists!", layout.id);
			return;
		}

		m_LayoutMap[layout.deviceClass].insert({ layout.id, layout });
	}

	void DeviceManager::AddDevice(const DeviceInfo& deviceInfo)
	{
		if (m_DeviceMap[deviceInfo.deviceClass].contains(deviceInfo.id))
		{
			INDY_CORE_ERROR("Could not add device. Device with ID [{0}] already exists!", deviceInfo.id);
			return;
		}

		const Layout* layout = MatchDeviceLayout(deviceInfo);

		if (!m_DeviceMap.contains(deviceInfo.deviceClass))
		{
			// Create the device map for this classification of device.
			std::unordered_map<uint32_t, Device> devices;
			devices.insert({deviceInfo.id, m_DeviceBuilder->Build(deviceInfo, *layout)});

			// Insert the device classification map into the device registry map.
			m_DeviceMap.insert({ deviceInfo.deviceClass, devices });
			return;
		}

		Device device = m_DeviceBuilder->Build(deviceInfo, *layout);
		m_DeviceMap[deviceInfo.deviceClass].insert({ deviceInfo.id, device });
	}

	Device* DeviceManager::GetDevice(const uint32_t& id)
	{
		for (auto& classPair : m_DeviceMap)
		{
			for (auto& pair : classPair.second)
			{
				if (pair.first == id)
					return &pair.second;
			}
		}

		return nullptr;
	}

	Device* DeviceManager::GetDevice(const std::string& name)
	{
		for (auto& classPair : m_DeviceMap)
		{
			for (auto& pair : classPair.second)
			{
				if (pair.second.GetName() == name)
					return &pair.second;
			}
		}

		return nullptr;
	}

	Device* DeviceManager::GetDevice(const DeviceInfo& deviceInfo)
	{
		// If a device class wasn't specified, try the id or display name
		if (deviceInfo.deviceClass == UINT16_MAX)
		{
			if (deviceInfo.id != UINT32_MAX)
				return GetDevice(deviceInfo.id);

			return GetDevice(deviceInfo.displayName);
		}

		// If a device class was specified, make sure it exists
		auto classIt = m_DeviceMap.find(deviceInfo.deviceClass);
		if (classIt == m_DeviceMap.end())
		{
			INDY_CORE_ERROR("Could not find device with classification [{0}].", deviceInfo.deviceClass);
			return nullptr;
		}

		// Then loop through all devices in that class and compare ID and display name.
		for (auto& pair : classIt->second)
		{
			if (pair.second.GetName() == deviceInfo.displayName ||
				pair.second.GetID() == deviceInfo.id)
				return &pair.second;
		}

		return nullptr;
	}

	const Layout* DeviceManager::MatchDeviceLayout(const DeviceInfo& deviceInfo)
	{
		// If a device class was specified, make sure it exists
		auto classIt = m_LayoutMap.find(deviceInfo.deviceClass);
		if (classIt == m_LayoutMap.end())
		{
			INDY_CORE_ERROR("Could not find layout with device classification [{0}].", deviceInfo.deviceClass);
			return nullptr;
		}

		auto layoutIt = classIt->second.find(deviceInfo.layoutID);
		if (layoutIt == classIt->second.end())
		{
			INDY_CORE_ERROR("Could not find layout with ID [{0}].", deviceInfo.layoutID);
			return nullptr;
		}

		return &layoutIt->second;
	}
}
