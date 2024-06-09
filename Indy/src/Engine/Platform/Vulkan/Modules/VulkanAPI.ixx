module;

#include <memory>
#include <string>
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

		private:
			VkInstance m_Instance;
			VkDebugUtilsMessengerEXT m_DebugMessenger;

			std::unique_ptr<VulkanRenderer> m_TestRenderer;
		};
	}
}