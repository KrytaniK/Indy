module;

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Events;

import Indy.Events;
import Indy.Window;

export
{
	namespace Indy
	{
		struct VkInstanceFetchEvent : IEvent
		{
			VkInstance* outInstance;
		};
	}
}