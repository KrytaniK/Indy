module;

#include <vector>
#include <string>

export module Indy_Core_InputSystem:LayoutMatcher;

import :Device;
import :DeviceControl;

export
{
	namespace Indy
	{ 
		// A template structure for building virtual devices. Tells the system how the device is laid out in memory.
		struct InputDeviceLayoutInfo
		{
			std::string vendorID; // [Optional] Vendor-Specific string ID (e.g., "0x54C" for Sony Entertainment)
			std::string productID; // [Optional] Device-Specific Product ID (e.g., "0x9CC" for wireless controller)
			std::string classification;
			std::vector<InputDeviceControlInfo> controls;
			int format; // unique identifier for the format of this layout
		};

		class InputLayoutMatcher
		{
		public:
			static InputDeviceLayoutInfo* Match(InputDeviceInfo* deviceInfo);

			static void RegisterLayout(const InputDeviceLayoutInfo& layoutInfo);

		private:
			static std::vector<InputDeviceLayoutInfo> s_Layouts;
		};
	}

	
}