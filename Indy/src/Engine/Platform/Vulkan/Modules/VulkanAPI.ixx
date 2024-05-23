module;

#include <vector>
#include <memory>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Backend;

import Indy.Graphics;

import :Events;
import :Device;

export
{
	namespace Indy::Graphics
	{
		class VulkanAPI : public IRenderAPI
		{
		public:
			VulkanAPI();
			virtual ~VulkanAPI() override;

			void OnLoad() override;
			void OnStart() override;
			void OnUnload() override;

			void OnDeviceSelect(VKDeviceSelectEvent* event);
			void OnSurfaceCreate(VKSurfaceCreateEvent* event);

		private:
			bool Init();
			void Cleanup();
			bool SupportsValidationLayers();

		private:
			VkInstance m_Instance;
			std::vector<std::shared_ptr<VulkanPhysicalDevice>> m_PhysicalDevices;
			VkDebugUtilsMessengerEXT m_DebugMessenger;
		};
	}
}