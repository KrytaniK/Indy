#include "Engine/Core/LogMacros.h"

#include <queue>
#include <functional>
#include <memory>

import Indy_Core_InputSystem;

namespace Indy
{
	std::queue<InputDeviceFactory::FactoryBlueprint> InputDeviceFactory::s_DeviceQueue;
	bool InputDeviceFactory::s_IsRunning;

	void InputDeviceFactory::Enqueue(InputDeviceInfo* deviceInfo, std::function<void(InputDeviceFactoryResult&)> callback)
	{
		s_DeviceQueue.push({std::make_unique<InputDeviceInfo>(*deviceInfo), callback});

		if (!s_IsRunning)
		{
			s_IsRunning = true;
			BuildDevice();
		}
	}

	void InputDeviceFactory::BuildDevice()
	{
		// This needs to operate on a separate thread.

		InputDeviceFactoryResult result{};

		InputDeviceLayoutInfo* deviceLayout = InputLayoutMatcher::Match(s_DeviceQueue.front().deviceInfo.get());

		// Ownership is passed to InputManager
		result.device = new InputDevice(*s_DeviceQueue.front().deviceInfo);

		result.success =
			deviceLayout != nullptr
			&& AllocateDeviceState(deviceLayout, result.device)
			&& CreateDeviceControls(deviceLayout, result.device);

		delete deviceLayout;

		s_DeviceQueue.front().callback(result);

		s_DeviceQueue.pop();

		if (!s_DeviceQueue.empty())
			BuildDevice();
		else
			s_IsRunning = false;
	}

	bool InputDeviceFactory::AllocateDeviceState(InputDeviceLayoutInfo* layout, InputDevice* outDevice)
	{
		if (layout == nullptr)
		{
			INDY_CORE_WARN("Layout is null");
			return false;
		}

		// Calculate total bit size of layout
		size_t stateSizeInBits = 0;
		for (auto& controlInfo : layout->controls)
		{
			stateSizeInBits += controlInfo.sizeInBits;
		}

		size_t stateSizeInBytes = (stateSizeInBits / 8) + 1; // 1 byte padding to prevent out-of-range errors

		// Set device state to raw memory block with layout's byte size
		outDevice->UpdateDeviceState(new char[stateSizeInBytes], 0, stateSizeInBytes);

		return true;
	}

	bool InputDeviceFactory::CreateDeviceControls(InputDeviceLayoutInfo* layout, InputDevice* outDevice)
	{
		if (layout == nullptr) 
		{
			INDY_CORE_ERROR("[InputDeviceFactory] Cannot create device controls... Layout is null.");
			return false;
		}

		// Controls should be sorted by this point, according to their byte offsets
		for (auto& controlInfo : layout->controls)
		{
			outDevice->AddControl(controlInfo);
		}

		return true;
	}
}