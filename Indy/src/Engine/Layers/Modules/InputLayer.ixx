module;

#include <memory>
#include <string>

export module Indy_Core:InputLayer;

import Indy_Core_Input;

import :LayerStack;

export
{
	namespace Indy
	{
		struct InputNotifyData : ILayerData
		{
			uint16_t deviceClass = 0xFFFF; // 2-byte (0-65535) Hex value for device classification (e.g., 0x00 for Pointer, 0x01 for Keyboard).
			uint16_t layoutClass = 0xFFFF; // 2-byte (0-65535) Hex value for device layout classification, relative to the device classification (e.g., 0x00 for Mouse, 0x01 for Touchpad).

			std::string controlName = ""; // The name of the target control. Used if controlIndex is not specified.
			uint16_t controlIndex = 0xFFFF; // The index of the target control.

			bool isComplete = false; // Determines whether this update alters device state completely.
		};

		struct InputCreateData : ILayerData
		{
			DeviceInfo* deviceInfo = nullptr;
			DeviceLayout* deviceLayout = nullptr;
		};

		class InputLayer : public ILayer
		{
		public:
			enum EventActions : uint8_t { Notify = 0x00, Create };

		public:
			virtual void onAttach() override;
			virtual void onDetach() override;
			virtual void Update() override;

		private:
			virtual void onEvent(LayerEvent& event) override;

			void ProcessInput(const InputNotifyData& event);

		private:
			std::unique_ptr<DeviceManager> m_DeviceManager;
		};
	}
}