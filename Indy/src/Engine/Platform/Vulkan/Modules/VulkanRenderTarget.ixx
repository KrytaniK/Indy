module;

#include <cstdint>
#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:RenderTarget;

import Indy.Graphics;
import Indy.Window;

import :Device;
import :Swapchain;

export
{
	namespace Indy
	{
		class VulkanRenderTarget : RenderTarget
		{
		public:
			VulkanRenderTarget(const VkInstance& instance, const GPUCompatibility& compatibility, Window* window = nullptr);
			virtual ~VulkanRenderTarget() override;

			virtual const uint32_t& GetID() const override { return m_ID; };

		private:
			uint32_t m_ID;
			VkInstance m_Instance;
			VkSurfaceKHR m_Surface;
			std::unique_ptr<VulkanDevice> m_Device;
			std::unique_ptr<VulkanSwapchain> m_Swapchain;
		};
	}
}