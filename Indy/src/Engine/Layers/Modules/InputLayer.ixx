module;

#include <memory>
#include <string>
#include <map>

export module Indy_Core:InputLayer;

import Indy_Core_Input;

import :LayerStack;

export
{
	namespace Indy
	{
		struct InputNotifyInfo : ILayerData
		{
			uint16_t deviceClass = 0xFFFF; // 2-byte (0-65535) Hex value for device classification (e.g., 0x00 for Pointer, 0x01 for Keyboard).
			uint16_t layoutClass = 0xFFFF; // 2-byte (0-65535) Hex value for device layout classification, relative to the device classification (e.g., 0x00 for Mouse, 0x01 for Touchpad).

			std::string device = ""; // The name of the target device.
			std::string control = ""; // The name of the target control.

			bool isComplete = false; // Determines whether this update alters device state completely.
		};

		struct InputCreateInfo : ILayerData
		{
			DeviceInfo* deviceInfo = nullptr;
			DeviceLayout* deviceLayout = nullptr;
		};

		struct InputWatchInfo : ILayerData
		{
			uint16_t deviceClass = 0xFFFF;
			uint16_t layoutClass = 0xFFFF;

			std::string device = ""; // The name of the target device.
			std::string control = ""; // The name of the target control.

			ControlContextCallback callback;
		};

		class InputLayer : public ILayer
		{
		public:
			enum EventActions : uint8_t { Notify = 0x00, Create, Watch };

		public:
			virtual void onAttach() override;
			virtual void onDetach() override;
			virtual void Update() override;

		private:
			virtual void onEvent(LayerEvent& event) override;

			void ProcessInput(const InputNotifyInfo& event);

		private:
			std::unique_ptr<DeviceManager> m_DeviceManager;
		};
	}
}