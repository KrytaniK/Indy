module;

#include <cstdint>
#include <string>
#include <memory>

export module Indy_Core_InputLayer:Actions;

import Indy_Core_LayerStack;
import Indy_Core_Input;

export
{
	namespace Indy
	{
		// ----- Layer Action Data -----

		// Input Action Data
		struct InputActionData : ILayerData
		{
			std::weak_ptr<DeviceManager> deviceManager;
		};

		// Input Action data for updating device controls
		struct AD_InputUpdateInfo : InputActionData
		{
			uint16_t deviceClass = 0xFFFF; // 2-byte (0-65535) Hex value for device classification (e.g., 0x00 for Pointer, 0x01 for Keyboard).
			uint16_t layoutClass = 0xFFFF; // 2-byte (0-65535) Hex value for device layout classification, relative to the device classification (e.g., 0x00 for Mouse, 0x01 for Touchpad).

			std::string device = ""; // The name of the target device.
			std::string control = ""; // The name of the target control.

			bool isPartial = true; // Determines whether this update alters device state completely.
			void* newState = nullptr;
		};

		// Input Action data for creating devices
		struct AD_InputCreateDeviceInfo : InputActionData
		{
			DeviceInfo* deviceInfo = nullptr;
		};

		// Input Action data for creating device layouts
		struct AD_InputCreateLayoutInfo : InputActionData
		{
			DeviceLayout* layout = nullptr;
		};

		// Input Action data for reacting to device state updates
		struct AD_InputWatchControlInfo : InputActionData
		{
			uint16_t deviceClass = 0xFFFF;
			uint16_t layoutClass = 0xFFFF;

			std::string device = ""; // The name of the target device.
			std::string control = ""; // The name of the target control.

			ControlContextCallback callback;
		};

		// ----- Layer Actions -----

		// Input Layer Action for updating device controls
		class LA_InputUpdate : public ILayerAction
		{
		public:
			virtual void Execute(ILayerData* layerData) override;
		};

		// Input Layer Action for creating devices
		class LA_InputCreateDevice : public ILayerAction
		{
		public:
			virtual void Execute(ILayerData* layerData) override;
		};

		// Input Layer action for creating device layouts
		class LA_InputCreateLayout : public ILayerAction
		{
		public:
			virtual void Execute(ILayerData* layerData) override;
		};

		// Input Layer Action for 'watching' device controls
		class LA_InputWatchControl : public ILayerAction
		{
		public:
			virtual void Execute(ILayerData* layerData) override;
		};
	}
}