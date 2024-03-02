#include "Engine/Core/LogMacros.h"

import Indy_Core_InputSystem;

namespace Indy
{
	std::vector<InputDeviceLayoutInfo> InputLayoutMatcher::s_Layouts;

	InputDeviceLayoutInfo* InputLayoutMatcher::Match(InputDeviceInfo* deviceInfo)
	{
		// Note: There may be cases where two or more layouts match 100% with
		//			with a device. In this case, only the first layout is
		//			returned.

		float percentMatch = 0.0f;
		InputDeviceLayoutInfo* outLayout = new InputDeviceLayoutInfo();

		// Perform an exhaustive search through all known device layouts.
		float layoutPercentMatch = 0.0f;
		for (InputDeviceLayoutInfo layout : s_Layouts)
		{
			// Assign weights (totalling 100) to each struct element.
			// Add weights to percentMatch if elements match EXACTLY.

			if (deviceInfo->vendorID == layout.vendorID)
				layoutPercentMatch += 25.f;

			if (deviceInfo->productID == layout.productID)
				layoutPercentMatch += 25.f;

			if (deviceInfo->classification == layout.classification)
				layoutPercentMatch += 25.f;

			if (deviceInfo->format == layout.format)
				layoutPercentMatch += 25.f;

			// Only layouts with higher percent matches are valid.
			if (layoutPercentMatch < percentMatch)
				continue;

			// A better layout was found for this device.
			percentMatch = layoutPercentMatch;
			layoutPercentMatch = 0.0f;
			*outLayout = layout;
		}

		if (percentMatch < 75.0f)
		{
			INDY_CORE_WARN("No suitable layout found for device [{0}]", deviceInfo->productID);
			return nullptr;
		}

		// Return the layout with the highest match percentage
		return outLayout;
	}

	void InputLayoutMatcher::RegisterLayout(const InputDeviceLayoutInfo& layoutInfo)
	{
		s_Layouts.emplace_back(layoutInfo);
	}
}