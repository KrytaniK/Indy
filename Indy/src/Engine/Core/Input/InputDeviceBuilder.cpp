#include <vector>

import Indy.Input;

namespace Indy::Input
{
	Device DeviceBuilder::Build(const DeviceInfo& deviceInfo, const Layout& deviceLayout)
	{
		// Create the device
		Device device(deviceInfo, deviceLayout.sizeInBytes);

		uint32_t controlCount = 0;
		// Build the device controls, only one level deep
		for (size_t i = 0; i < deviceLayout.controls.size();)
		{
			// Temp control info
			ControlInfo controlInfo = deviceLayout.controls[i];
			controlInfo.id = controlCount++;

			// Create and attach control to device
			Control* baseControl = device.AddControl(controlInfo);

			// Create child controls
			if (controlInfo.childCount > 0)
			{
				for (uint8_t j = 1; j <= controlInfo.childCount; j++)
				{
					ControlInfo childControlInfo = deviceLayout.controls[i + j];
					childControlInfo.id = controlCount++;

					baseControl->AddChild(childControlInfo);
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
}
