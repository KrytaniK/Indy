module;

#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:RenderTarget;

import Indy.Graphics;
import Indy.Window;

import :Pipeline;
import :Device;
import :Swapchain;
import :Frame;
import :Image;
import :Descriptor;

export
{
	namespace Indy
	{
		struct VulkanRTSpec
		{
			VulkanDevice* deviceHandle;
			bool useSurface = false;
			Window* window = nullptr;
			VulkanDescriptor* imageDescriptor;
			VulkanPipeline* computePipeline;
		};

		class VulkanRenderTarget : RenderTarget
		{
		public:
			VulkanRenderTarget(const VkInstance& instance, const VulkanRTSpec& spec);
			virtual ~VulkanRenderTarget() override;

			virtual const uint32_t& GetID() const override { return m_ID; };

			void Render();

		private:
			// Frame preparations
			void OnBeginFrame(VulkanFrame* frame, uint32_t& swapchainImageIndex);

			// Vulkan commands are issued here
			void OnDrawFrame(VulkanFrame* frame, const uint32_t& swapchainImageIndex);

			// Finalize commands and prepare for presentation
			void OnEndFrame(VulkanFrame* frame);

			// Present finalized frame
			void OnPresentFrame(VulkanFrame* frame, const uint32_t& swapchainImageIndex);

			// Retrieve the current frame in the "frame buffer"
			VulkanFrame* GetCurrentFrame();

		private:
			uint32_t m_ID;
			VkInstance m_Instance;
			VkSurfaceKHR m_Surface;

			VulkanDevice* m_DeviceHandle;
			std::unique_ptr<VulkanSwapchain> m_Swapchain;
			std::unique_ptr<VulkanImage> m_RenderImage;

			VulkanPipeline* m_ComputePipeline;
			VulkanDescriptor* m_RenderImageDescriptor;

			std::vector<std::unique_ptr<VulkanFrame>> m_Frames;
			uint8_t m_FrameCount;
		};
	}
}