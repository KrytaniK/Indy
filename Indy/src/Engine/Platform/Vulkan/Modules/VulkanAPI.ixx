module;

#include <memory>
#include <vector>
#include <map>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Backend;

import Indy.Graphics;
import Indy.Window;

import :Device;
import :DescriptorPool;
import :Pipeline;
import :Renderer;

export
{
	namespace Indy
	{
		class VulkanAPI : public GraphicsAPI
		{
		private:
			inline static bool s_IsInitialized = false;

		public:
			VulkanAPI();
			virtual ~VulkanAPI() override;

			virtual void OnLoad() override;
			virtual void OnStart() override;
			virtual void OnUnload() override;

			virtual void OnWindowDispatch(WindowDispatchEvent* event) override;

		private:
			bool InitVulkan();
			void Cleanup();
			bool SupportsValidationLayers();

			bool CreateVulkanInstance();
			bool CreateDebugMessenger(const VkDebugUtilsMessengerCreateInfoEXT& createInfo);

			std::shared_ptr<VulkanDevice> CreateVulkanDevice();

		private:
			VkInstance m_Instance;
			VkDebugUtilsMessengerEXT m_DebugMessenger;

			std::unique_ptr<VulkanRenderer> m_Renderer;
		};
	}
}