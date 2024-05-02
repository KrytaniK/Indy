#include <memory>

import Indy_Core_Input;

namespace Indy
{
	std::shared_ptr<Device> DeviceBuilder::Build(const DeviceInfo& deviceInfo, const DeviceLayout& deviceLayout)
	{
		// Create temporary vectors for storing controls.
		std::vector<std::shared_ptr<DeviceControl>> controls;
		std::vector<std::shared_ptr<DeviceControl>> childControls;

		// Build the device controls, only one level deep
		for (size_t i = 0; i < deviceLayout.controls.size();)
		{
			childControls.clear();

			// Temp control info
			DeviceControlInfo controlInfo = deviceLayout.controls[i];

			// Create child controls
			if (controlInfo.childCount > 0)
			{
				childControls.reserve(controlInfo.childCount);

				for (uint8_t j = 1; j <= controlInfo.childCount; j++)
				{
					// Create a child control and pass ownership to this control.
					childControls.emplace_back(
						std::make_shared<DeviceControl>(deviceLayout.controls[i + j])
					);
				}

				i += (size_t)(controlInfo.childCount + 1);
			}
			else
			{
				i++;
			}

			controls.emplace_back(std::make_shared<DeviceControl>(controlInfo, childControls));
		}

		return std::make_shared<Device>(deviceInfo, deviceLayout.sizeInBytes, controls);
	}

	std::vector<std::shared_ptr<Device>> DeviceBuilder::Build(const std::vector<std::pair<const DeviceInfo&, const DeviceLayout&>>& blueprints)
	{
		std::vector<std::shared_ptr<Device>> devices;
		devices.reserve(blueprints.size());

		for (const auto& deviceBlueprint : blueprints)
			devices.emplace_back(
				Build(deviceBlueprint.first, deviceBlueprint.second)
			);

		return devices;
	}

	

}