#include <Engine/Core/LogMacros.h>

#include <memory>

import Indy_Core_InputSystem;

namespace Indy
{
	InputDeviceControl::InputDeviceControl(const InputDeviceControlInfo& info, char* deviceState)
		: m_Info(info)
	{
		m_Value = std::make_unique<InputDeviceControlValue>(info.Size(), deviceState + info.offset, info.bit == 255 ? -1 : info.bit);
	}

	InputDeviceControl::~InputDeviceControl()
	{
		
	}

	void InputDeviceControl::Update()
	{
		for (auto& callback : m_Callbacks)
			callback(*m_Value);
	}

	void InputDeviceControl::Watch(const std::function<void(const InputDeviceControlValue&)>& onValueChange)
	{
		m_Callbacks.emplace_back(onValueChange);
	}

	InputDeviceControlValue::InputDeviceControlValue(size_t size, char* controlState, int bit)
		: m_Size(size), m_RawValue(controlState), m_Bit(bit)
	{
		
	}

	InputDeviceControlValue::~InputDeviceControlValue()
	{

	}
}