module;

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Backend;

import Indy.Graphics;
import Indy.Window;

import :Events;
import :RenderTarget;
import :Device;

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

		private:
			VkInstance m_Instance;
			VkDebugUtilsMessengerEXT m_DebugMessenger;
		};
	}
}