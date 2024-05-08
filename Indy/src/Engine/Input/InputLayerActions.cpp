#include "Engine/Core/LogMacros.h"

#include <string>

import Indy_Core_InputLayer;
import Indy_Core_Input;

namespace Indy
{
	void ICL_InputAction_Update::Execute(ILayerData* layerData)
	{
		// Extract Input Update Data
		ICL_InputData_Update* actionData = static_cast<ICL_InputData_Update*>(layerData);

		// Bail if the device manager is invalid
		if (!m_DeviceManager)
		{
			INDY_CORE_ERROR("[ICL_InputAction_Update] Failed to process input. Bad Device Manager...");
			return;
		}

		m_DeviceManager->UpdateDeviceState(
			actionData->targetDevice, 
			actionData->targetControl, 
			(std::byte*)actionData->newState
		);
	}

	void ICL_InputAction_CreateDevice::Execute(ILayerData* layerData)
	{
		// Extract Input Update Data
		ICL_InputData_CreateDevice* actionData = static_cast<ICL_InputData_CreateDevice*>(layerData);

		// Bail if the device manager is invalid
		if (!m_DeviceManager)
		{
			INDY_CORE_ERROR("[ICL_InputAction_CreateDevice] Failed to create device. Bad Device Manager...");
			return;
		}

		m_DeviceManager->AddDevice(*actionData->deviceInfo);
	}

	void ICL_InputAction_CreateLayout::Execute(ILayerData* layerData)
	{
		// Extract Input Update Data
		ICL_InputData_CreateLayout* actionData = static_cast<ICL_InputData_CreateLayout*>(layerData);

		// Bail if the device manager is invalid
		if (!m_DeviceManager)
		{
			INDY_CORE_ERROR("[ICL_InputAction_CreateLayout] Failed to create layout. Bad Device Manager...");
			return;
		}

		m_DeviceManager->AddLayout(*actionData->layout);
	}

	void ICL_InputAction_WatchControl::Execute(ILayerData* layerData)
	{
		// Extract Input Update Data
		ICL_InputData_WatchControl* actionData = static_cast<ICL_InputData_WatchControl*>(layerData);

		// Bail if the device manager is invalid
		if (!m_DeviceManager)
		{
			INDY_CORE_ERROR("Failed to watch control. Bad Device Manager...");
			return;
		}

		m_DeviceManager->WatchDeviceControl(
			actionData->targetDevice,
			actionData->targetControl, 
			actionData->callback
		);
	}
}