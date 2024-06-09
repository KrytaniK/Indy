module;

#include <vector>
#include <queue>
#include <memory>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

export module Indy.VulkanGraphics:Renderer;

import :Device;
import :Swapchain;
import :Frame;
import :Image;
import :Pipeline;
import :Descriptor;
import :DescriptorPool;
import :CommandPool;

import Indy.Graphics;
import Indy.Window;

export
{
	namespace Indy
	{

		struct VulkanRendererPipelineInfo
		{
			std::unique_ptr<VulkanDescriptorPool> descriptorPool;

			std::unique_ptr<VulkanPipeline> compute;
			std::unique_ptr<VulkanPipeline> graphics;
			std::unique_ptr<VulkanPipeline> raytrace;

			std::shared_ptr<VulkanDescriptor> computeDescriptor;
			std::shared_ptr<VulkanDescriptor> graphicsDescriptor;
			std::shared_ptr<VulkanDescriptor> raytraceDescriptor;
		};

		class VulkanRenderer : public Renderer
		{
		public:
			VulkanRenderer(Window* window, const VkInstance& instance);
			virtual ~VulkanRenderer() override;

			virtual void Render() override;
			
			virtual void Enable() override;
			virtual void Disable() override;

		private:
			void BuildPipelines();
			void InitImGui(Window* window, const VkInstance& instance);
			void RenderImGui(const VkCommandBuffer& cmdBuffer, const VkImageView& imageView);

		private:
			bool m_Enabled;
			uint8_t m_ID;
			uint8_t m_CurrentFrameIndex;
			std::vector<VulkanFrameData> m_Frames; // The "in-flight" frames we're recording

			VkInstance m_Instance;
			std::unique_ptr<VulkanDevice> m_Device; // Containing Vulkan Device
			VkDevice m_LogicalDevice; // A reference to the logical device
			VulkanPhysicalDevice m_PhysicalDevice; // A reference to the physical device (GPU)

			VkSurfaceKHR m_Surface; // The surface the render image is presented to
			uint32_t m_SwapchainImageIndex; // Index of the next available swapchain image
			std::unique_ptr<VulkanSwapchain> m_Swapchain; // Utility for Vulkan's VkSwapchainKHR

			VkQueue m_ComputeQueue;
			VkQueue m_GraphicsQueue;
			VkQueue m_PresentQueue;

			VulkanImage m_RenderImage; // The image this renderer records commands on
			VmaAllocator m_ImageAllocator; // Vulkan Memory Allocator for the Render Image
			VulkanImageProcessor m_ImageProcessor; // Member for transitioning and copying images and image layouts

			VulkanRendererPipelineInfo m_Pipelines{}; // Pipeline information for this renderer

			// Immediate Commands
			VulkanCommandPool m_ImmediateCmdPool;
			VkCommandBuffer m_ImmediateCmdBuffer;
			VkFence m_ImmediateFence;

			// ImGui
			VkDescriptorPool m_ImGuiDescriptorPool;
		};
	}
}