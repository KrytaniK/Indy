#include "Engine/Core/LogMacros.h"

#include <string>

import Indy_Core_InputLayer;
import Indy_Core_Input;

namespace Indy
{
	void LA_InputUpdate::Execute(ILayerData* layerData)
	{
		// Extract Input Update Data
		AD_InputUpdateInfo* actionData = static_cast<AD_InputUpdateInfo*>(layerData);

		// Extract Device Manager
		std::weak_ptr<DeviceManager> deviceManager = actionData->deviceManager;

		// Bail if the device manager is invalid
		if (deviceManager.expired())
		{
			INDY_CORE_ERROR(
				"Failed to process input. Bad Device Manager... \n[Device Name] {0}\n[Device Class] {1}\n[Device Layout] {2}",
				actionData->device,
				actionData->deviceClass,
				actionData->layoutClass
			);
			return;
		}

		// Search for the device
		std::weak_ptr<Device> device = actionData->device != "" ?
			deviceManager.lock()->GetDevice(actionData->device) :
			deviceManager.lock()->GetDevice(actionData->deviceClass, actionData->layoutClass);

		// Bail if the device is invalid
		if (device.expired())
		{
			INDY_CORE_ERROR(
				"Failed to process input. Device does not exists... \n[Device Name] {0}\n[Device Class] {1}\n[Device Layout] {2}", 
				actionData->device, 
				actionData->deviceClass, 
				actionData->layoutClass
			);
			return;
		}

		// Update all of the device state if the update is not partial
		if (!actionData->isPartial)
		{
			device.lock()->Update(static_cast<std::byte*>(actionData->newState));
			return;
		}

		// If the update is partial, extract the control
		std::weak_ptr<DeviceControl> control = device.lock()->GetControl(actionData->control);

		// Bail if the control is invalid
		if (control.expired())
		{
			INDY_CORE_ERROR(
				"Failed to process input. Device Control does not exists... \n[Control Name] {0}\n", 
				actionData->control
			);
			return;
		}

		// Update control state
		control.lock()->Update(static_cast<std::byte*>(actionData->newState));
	}

	void LA_InputCreateDevice::Execute(ILayerData* layerData)
	{
		// Extract Input Update Data
		AD_InputCreateDeviceInfo* actionData = static_cast<AD_InputCreateDeviceInfo*>(layerData);

		// Extract Device Manager
		std::weak_ptr<DeviceManager> deviceManager = actionData->deviceManager;

		// Bail if the device manager is invalid
		if (deviceManager.expired())
		{
			INDY_CORE_ERROR("Failed to create device. Bad Device Manager...");
			return;
		}

		deviceManager.lock()->AddDevice(*actionData->deviceInfo);
	}

	void LA_InputCreateLayout::Execute(ILayerData* layerData)
	{
		// Extract Input Update Data
		AD_InputCreateLayoutInfo* actionData = static_cast<AD_InputCreateLayoutInfo*>(layerData);

		// Extract Device Manager
		std::weak_ptr<DeviceManager> deviceManager = actionData->deviceManager;

		// Bail if the device manager is invalid
		if (deviceManager.expired())
		{
			INDY_CORE_ERROR("Failed to create device. Bad Device Manager...");
			return;
		}

		deviceManager.lock()->AddLayout(*actionData->layout);
	}

	void LA_InputWatchControl::Execute(ILayerData* layerData)
	{
		// Extract Input Update Data
		AD_InputWatchControlInfo* actionData = static_cast<AD_InputWatchControlInfo*>(layerData);

		// Extract Device Manager
		std::weak_ptr<DeviceManager> deviceManager = actionData->deviceManager;

		// Bail if the device manager is invalid
		if (deviceManager.expired())
		{
			INDY_CORE_ERROR("Failed to create device. Bad Device Manager...");
			return;
		}

		// Extract the device
		std::weak_ptr<Device> device = actionData->device != "" ?
			deviceManager.lock()->GetDevice(actionData->device) :
			deviceManager.lock()->GetDevice(actionData->deviceClass, actionData->layoutClass);

		// Bail if the device is invalid
		if (device.expired())
		{
			INDY_CORE_ERROR(
				"Failed to process input. Device does not exists... \n[Device Name] {0}\n[Device Class] {1}\n[Device Layout] {2}",
				actionData->device,
				actionData->deviceClass,
				actionData->layoutClass
			);
			return;
		}

		// Extract the control
		std::weak_ptr<DeviceControl> control = device.lock()->GetControl(actionData->control);

		// Bail if the control is invalid
		if (control.expired())
		{
			INDY_CORE_ERROR(
				"Failed to process input. Device Control does not exists... \n[Control Name] {0}\n",
				actionData->control
			);
			return;
		}

		// Watch control state
		control.lock()->Watch(actionData->callback);
	}
}