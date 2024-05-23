module;

#include <memory>

export module Indy.VulkanGraphics:Events;

import Indy.Events;
import Indy.Window;

export
{
	namespace Indy::Graphics
	{
		struct VKDeviceCompat;
		struct VulkanPhysicalDevice;

		struct VKDeviceSelectEvent : IEvent
		{
			const VKDeviceCompat* compatCriteria;
			std::shared_ptr<VulkanPhysicalDevice> outDevice;
		};

		struct VKSurfaceCreateEvent : IEvent
		{
			IWindow* window;
			VKDeviceCompat* compat;
		};
	}
}