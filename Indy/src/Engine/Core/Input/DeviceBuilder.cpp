#include <memory>

import Indy.Input;

namespace Indy
{
	std::shared_ptr<Device> DeviceBuilder::Build(const DeviceInfo& deviceInfo, const DeviceLayout& deviceLayout)
	{
		// Create the device
		std::shared_ptr<Device> device = std::make_shared<Device>(deviceInfo, deviceLayout.sizeInBytes);

		// Build the device controls, only one level deep
		for (size_t i = 0; i < deviceLayout.controls.size();)
		{
			// Temp control info
			DeviceControlInfo controlInfo = deviceLayout.controls[i];

			// Create Base Control
			std::shared_ptr<DeviceControl> baseControl = std::make_shared<DeviceControl>(controlInfo);

			// Attach control to device
			device->AddControl(baseControl);

			// Create child controls
			if (controlInfo.childCount > 0)
			{
				for (uint8_t j = 1; j <= controlInfo.childCount; j++)
				{
					baseControl->AddChild(deviceLayout.controls[i + j]);
				}

				i += (size_t)(controlInfo.childCount + 1);
			}
			else
			{
				i++;
			}
		}

		return device;
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