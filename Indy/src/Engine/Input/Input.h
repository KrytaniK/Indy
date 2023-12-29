#pragma once

#include "Engine/EventSystem/Events.h"
#include "InputDescription.h"

#include <map>

namespace Engine
{
	class InputManager
	{
	private:
		static std::unordered_map<std::string, InputDevice> s_Devices;
		static std::vector<InputLayout> s_Layouts;

	public:
		static void Init();
		static void Shutdown();

		// Device Registration
		static void RegisterDevice(InputDevice device);

		// Device Retrieval
		static uint8_t GetDeviceID(std::string displayName);
		static InputDevice* GetDevice(std::string displayName);

		// Layout Register
		static uint16_t RegisterLayout(InputLayout layout);
		static uint16_t RegisterDeviceLayout(InputDevice device, InputLayout layout);
		static uint16_t RegisterDeviceLayout(std::string deviceName, InputLayout layout);

		// Layout Retrieval
		static uint16_t GetDeviceLayoutID(std::string deviceName, std::string layoutName);
		static InputLayout* GetDeviceLayout(std::string deviceName, std::string layoutName);
		static InputLayout* GetLayout(uint16_t index);

	private:
		static void OnInput(Event& event);
	};
}