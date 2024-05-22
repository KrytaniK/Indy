module;

#include <memory>

export module Indy.VulkanGraphics:Events;

import :Utils;

import Indy.Events;

export
{
	namespace Indy::Graphics
	{
		struct VulkanGPUEvent : IEvent
		{
			const VulkanDeviceCompatibility* compatibility = nullptr;
			std::shared_ptr<VulkanPhysicalDevice> outDevice;
		};
	}
}