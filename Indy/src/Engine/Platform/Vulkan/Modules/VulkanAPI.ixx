module;

#include <memory>
#include <string>
#include <unordered_map>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Backend;

import Indy.Graphics;
import Indy.Window;

import :Events;
import :RenderTarget;
import :Device;
import :Descriptor;
import :DescriptorPool;

export
{
	namespace Indy
	{
		class VulkanAPI : public GraphicsAPI
		{
		public:
			VulkanAPI();
			virtual ~VulkanAPI() override;

			virtual void OnLoad() override;
			virtual void OnStart() override;
			virtual void OnUnload() override;

			virtual void CreateRenderTarget(Window* window) override;

			void OnFetchInstance(VkInstanceFetchEvent* event);

		private:
			bool Init();
			void Cleanup();
			bool SupportsValidationLayers();

			bool CreateVulkanInstance();
			bool CreateDebugMessenger(const VkDebugUtilsMessengerCreateInfoEXT& createInfo);
			bool CreateGlobalDevice();
			bool CreateGlobalDescriptorPool();
			bool CreateBasePipelines();

		private:
			VkInstance m_Instance;
			VkDebugUtilsMessengerEXT m_DebugMessenger;
			std::unique_ptr<VulkanDevice> m_Global_Device;
			std::unique_ptr<VulkanDescriptorPool> m_Global_DescriptorPool;

			// Temp?
			std::unordered_map<std::string, std::unique_ptr<VulkanPipeline>> m_Pipelines;
		};
	}
}