#include <memory>
#include <vector>
#include <unordered_map>

import Indy.Input;

namespace Indy
{
	std::shared_ptr<InputDevice> InputDeviceBuilder::Build(const InputDeviceInfo& deviceInfo, const InputLayout& deviceLayout)
	{
		// Create the device
		std::shared_ptr<InputDevice> device = std::make_shared<InputDevice>(deviceInfo, deviceLayout.sizeInBytes);

		uint32_t controlCount = 0;
		// Build the device controls, only one level deep
		for (size_t i = 0; i < deviceLayout.controls.size();)
		{
			// Temp control info
			InputControlInfo controlInfo = deviceLayout.controls[i];

			// Create Base Control
			InputControl baseControl(controlInfo);

			// Attach control to device
			device->AddControl(baseControl);

			// Create child controls
			if (controlInfo.childCount > 0)
			{
				for (uint8_t j = 1; j <= controlInfo.childCount; j++)
				{

					baseControl.AddChild(deviceLayout.controls[i + j]);
				}

				i += static_cast<size_t>(controlInfo.childCount + 1);
			}
			else
			{
				i++;
			}
		}

		return device;
	}

	std::vector<std::shared_ptr<InputDevice>> InputDeviceBuilder::Build(const std::vector<std::pair<const InputDeviceInfo&, const InputLayout&>>& blueprints)
	{
		std::vector<std::shared_ptr<InputDevice>> devices;
		devices.reserve(blueprints.size());

		for (const auto& deviceBlueprint : blueprints)
			devices.emplace_back(
				Build(deviceBlueprint.first, deviceBlueprint.second)
			);

		return devices;
	}

	

}