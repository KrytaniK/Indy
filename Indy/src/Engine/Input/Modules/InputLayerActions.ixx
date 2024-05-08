module;

#include <cstdint>
#include <string>
#include <memory>
#include <functional>

export module Indy_Core_InputLayer:Actions;

import Indy_Core_LayerStack;
import Indy_Core_Input;

export
{
	namespace Indy
	{
		// ----- Layer Action Data -----

		// Input Action data for updating device controls
		struct ICL_InputData_Update : ILayerData
		{
			DeviceInfo* targetDevice;
			std::string targetControl;
			void* newState = nullptr;
		};

		// Input Action data for creating devices
		struct ICL_InputData_CreateDevice : ILayerData
		{
			DeviceInfo* deviceInfo = nullptr;
		};

		// Input Action data for creating device layouts
		struct ICL_InputData_CreateLayout : ILayerData
		{
			DeviceLayout* layout = nullptr;
		};

		// Input Action data for reacting to device state updates
		struct ICL_InputData_WatchControl : ILayerData
		{
			DeviceInfo* targetDevice;
			std::string targetControl;
			std::function<void(DeviceControlContext&)> callback;
		};

		// ----- Layer Actions -----

		// Input Layer Action for updating device controls
		class ICL_InputAction_Update : public ILayerAction
		{
		public:
			ICL_InputAction_Update(DeviceManager* deviceManager)
				: m_DeviceManager(deviceManager) {};

			virtual void Execute(ILayerData* layerData) override;

		private:
			// Associative reference to the device manager
			DeviceManager* m_DeviceManager;
		};

		// Input Layer Action for creating devices
		class ICL_InputAction_CreateDevice : public ILayerAction
		{
		public:
			ICL_InputAction_CreateDevice(DeviceManager* deviceManager)
				: m_DeviceManager(deviceManager) {};
			
			virtual void Execute(ILayerData* layerData) override;

		private:
			// Associative reference to the device manager
			DeviceManager* m_DeviceManager;
		};

		// Input Layer action for creating device layouts
		class ICL_InputAction_CreateLayout : public ILayerAction
		{
		public:
			ICL_InputAction_CreateLayout(DeviceManager* deviceManager)
				: m_DeviceManager(deviceManager) {};

			virtual void Execute(ILayerData* layerData) override;

		private:
			// Associative reference to the device manager
			DeviceManager* m_DeviceManager;
		};

		// Input Layer Action for 'watching' device controls
		class ICL_InputAction_WatchControl : public ILayerAction
		{
		public:
			ICL_InputAction_WatchControl(DeviceManager* deviceManager)
				: m_DeviceManager(deviceManager) {};

			virtual void Execute(ILayerData* layerData) override;

		private:
			// Associative reference to the device manager
			DeviceManager* m_DeviceManager;
		};
	}
}