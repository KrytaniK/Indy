module;

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:RenderTarget;

import Indy.Graphics;
import Indy.Window;

import :Device;
import :Swapchain;

export
{
	namespace Indy::Graphics
	{
		class VulkanRenderTarget : IRenderTarget
		{
		public:
			VulkanRenderTarget(const VkInstance& instance, const VKDeviceCompat& compatibility, IWindow* window = nullptr);
			virtual ~VulkanRenderTarget() override;

		private:
			std::unique_ptr<VulkanDevice> m_Device;
			std::unique_ptr<VulkanSwapchain> m_Swapchain;
			VkInstance m_Instance;
			VkSurfaceKHR m_Surface;
		};
	}
}