#include "Engine/Core/LogMacros.h"

#include <queue>
#include <memory>
#include <map>

import Indy_Core_InputSystem;
import Indy_Core_EventSystem;

namespace Indy
{
	std::map<int, InputDevice*> InputManager::s_Devices;

	InputManager::InputManager()
	{
		Enable();
	}

	InputManager::~InputManager()
	{
		Disable();

		for (auto& pair : s_Devices)
			delete pair.second;
	}

	void InputManager::RegisterLayout(const InputDeviceLayoutInfo& layout)
	{
		InputLayoutMatcher::RegisterLayout(layout);
	}

	void InputManager::Enable()
	{
		/*m_EventHandles.emplace_back(
			EventManager::AddEventListener<InputManager, InputDeviceDetectEvent>(this, &InputManager::OnDeviceDetected)
		);
		m_EventHandles.emplace_back(
			EventManager::AddEventListener<InputManager, InputDeviceEvent>(this, &InputManager::OnDeviceEvent)
		);*/
	}

	void InputManager::Disable()
	{
		for (const IEventHandle& handle : m_EventHandles)
			EventManager::RemoveEventListener(handle);

		m_EventHandles.clear();
	}

	const InputDeviceControlInfo& InputManager::GetDeviceControlInfo(int deviceFormat, const std::string& control)
	{
		auto deviceIt = s_Devices.find(deviceFormat);
		if (deviceIt == s_Devices.end())
		{
			INDY_CORE_CRITICAL("Device [Format {0}] does not exist!", deviceFormat);
			throw std::runtime_error("Input Device Not Found");
		}

		return deviceIt->second->GetControlInfo(control);
	}

	const InputDeviceControlInfo& InputManager::GetDeviceControlInfo(const std::string& deviceName, const std::string& control)
	{
		for (const auto& pair : s_Devices)
			if (pair.second->GetInfo().displayName == deviceName)
				return pair.second->GetControlInfo(control);

		INDY_CORE_CRITICAL("Device [{0}] does not exist!", deviceName);
		throw std::runtime_error("Could not find device...");
	}

	void InputManager::WatchDeviceControl(const InputDeviceControlInfo& controlInfo, const std::function<void(const InputDeviceControlValue&)>& onValueChange)
	{
		auto deviceIt = s_Devices.find(controlInfo.deviceFormat);
		if (deviceIt == s_Devices.end())
		{
			INDY_CORE_ERROR("Cannot watch device control [{0}]. Device does not exist! [{1}]", controlInfo.displayName, controlInfo.deviceFormat);
			return;
		}

		deviceIt->second->WatchControl(controlInfo, onValueChange);
	}

	void InputManager::ProcessEvents()
	{
		while (!m_DeviceEventQueue.empty())
		{
			auto deviceIt = s_Devices.find(m_DeviceEventQueue.front()->deviceFormat);
			if (deviceIt != s_Devices.end())
			{
				deviceIt->second->UpdateDeviceState(
					static_cast<char*>(m_DeviceEventQueue.front()->deviceState),
					m_DeviceEventQueue.front()->offset,
					m_DeviceEventQueue.front()->size,
					m_DeviceEventQueue.front()->bit
				);

				m_DeviceEventQueue.pop();
				continue;
			}
			else
				INDY_CORE_WARN("Device Not Found...");

			m_DeviceEventQueue.pop();
		}
	}

	void InputManager::OnDeviceDetected(InputDeviceDetectEvent* event)
	{
		InputDeviceFactory::Enqueue(&event->deviceInfo, [=](InputDeviceFactoryResult& result)
			{
				// To make this thread-safe, add thread locks for s_Devices.
				if (!result.success)
				{
					INDY_CORE_ERROR("[InputManager - DeviceFactory] Failed to create input device!");
					return;
				}

				s_Devices.insert(std::make_pair(result.device->GetFormat(), result.device));
			}
		);
	}

	void InputManager::OnDeviceEvent(InputDeviceEvent* event)
	{
		m_DeviceEventQueue.push(std::make_unique<InputDeviceEvent>(*event)); // to avoid a dangling pointer
	}
}