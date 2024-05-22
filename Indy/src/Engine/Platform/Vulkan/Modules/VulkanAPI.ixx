module;

#include <vector>
#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Backend;

import Indy.Graphics;

import :Events;
import :Utils;

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

		private:
			bool Init();
			void Cleanup();

			bool SupportsValidationLayers();
			std::vector<std::shared_ptr<VulkanPhysicalDevice>> GetAllPhysicalDevices(const VkInstance& instance);
			std::shared_ptr<VulkanPhysicalDevice> GetCompatibleDevice(const VulkanDeviceCompatibility& compatibility);
			uint8_t RateDeviceCompatibility(const VulkanPhysicalDevice& device, const VulkanDeviceCompatibility& compatibility);
			bool IsCompatibleFeature(const VulkanCompatibility& preference, bool hasFeature, uint8_t& rating);

			void OnChoosePhysicalDevice(VulkanGPUEvent* event);

		private:
			VkInstance m_Instance;
			std::vector<std::shared_ptr<VulkanPhysicalDevice>> m_PhysicalDevices;
			VkDebugUtilsMessengerEXT m_DebugMessenger;
		};
	}
}