module;

#include <cstdint>
#include <vector>
#include <functional>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:RenderTarget;

import Indy.Graphics;
import Indy.Window;

import :Pipeline;
import :Device;
import :Swapchain;
import :Frame;
import :Image;
import :DescriptorPool;
import :CommandPool;

export
{
	namespace Indy
	{
		struct VulkanRTSpec
		{
			VulkanDevice* deviceHandle;
			bool useSurface = false;
			Window* window = nullptr;
			VulkanPipeline* computePipeline;
			VulkanPipeline* graphicsPipeline;
			VulkanPipeline* raytracePipeline;
		};

		class VulkanRenderTarget : RenderTarget
		{
		public:
			VulkanRenderTarget(const VkInstance& instance, const VulkanRTSpec& spec);
			virtual ~VulkanRenderTarget() override;

			virtual const uint32_t& GetID() const override { return m_ID; };

			void Render();

		private:
			// TEMP: MOVE LATER
			void init_imgui(const VkInstance& instance, Window* window);
			void draw_imgui(const VkCommandBuffer& buffer, const VkImageView& imageView);
			// ------------------

			// Execute compute operations
			void OnBeginFrame(const FrameData& frameData);

			// Vulkan commands are issued here
			void OnDrawFrame(FrameData& frameData);

			// Finalize and present frame
			void OnPresentFrame(const FrameData& frameData);

			void SubmitComputeQueue(const FrameData& frameData);
			void SubmitGraphicsQueue(const FrameData& frameData, bool present = true);

			void SubmitImmediateCommand(std::function<void(const VkCommandBuffer&)>&& function);

			// Retrieve the current frame in the "frame buffer"
			FrameData GetCurrentFrameData();

		private:
			uint32_t m_ID;
			VkInstance m_Instance;
			VkSurfaceKHR m_Surface;
			VulkanDevice* m_DeviceHandle;

			std::unique_ptr<VulkanSwapchain> m_Swapchain;
			std::unique_ptr<VulkanImage> m_OffscreenImage;

			VulkanPipeline* m_ComputePipeline;

			std::vector<std::unique_ptr<VulkanFrame>> m_Frames;
			uint8_t m_FrameCount;

			std::unique_ptr<VulkanCommandPool> m_ImmediateCommandPool;
			std::unique_ptr<VulkanSyncObjects> m_ImmediateSyncObjects;

			// TEMP
			VkDescriptorPool m_ImGuiPool;
		};
	}
}