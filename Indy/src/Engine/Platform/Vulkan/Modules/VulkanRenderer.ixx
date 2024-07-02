module;

#include <vector>
#include <queue>
#include <memory>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <glm/glm.hpp>

export module Indy.VulkanGraphics:Renderer;

import :Device;
import :Swapchain;
import :Frame;
import :Image;
import :Pipeline;
import :Descriptor;
import :CommandPool;

import Indy.Graphics;
import Indy.Window;

export
{
	namespace Indy
	{
		class VulkanRenderer : public Renderer
		{
		public:
			VulkanRenderer(Window* window, const VkInstance& instance, const std::shared_ptr<VulkanDevice>& device);
			virtual ~VulkanRenderer() override;

			virtual void Render() override;
			
			virtual void Enable() override;
			virtual void Disable() override;

		private:
			void BuildPipelines();
			void InitImGui();
			void RenderImGui(const VkCommandBuffer& cmdBuffer, const VkImageView& imageView);

		private:
			VkInstance m_Instance;
			std::shared_ptr<VulkanDevice> m_Device;

			Window* m_Window;
			VkSurfaceKHR m_Surface;
			VulkanSwapchain m_Swapchain;

			VmaAllocator m_ImageAllocator;
			VulkanImage m_RenderImage;
			VulkanImageProcessor m_ImageProcessor;
			bool m_RenderAsUITexture = true;

			VkQueue m_ComputeQueue;
			VkQueue m_GraphicsQueue;
			VkQueue m_PresentQueue;

			std::vector<VulkanFrameData> m_Frames;
			uint32_t m_CurrentFrameIndex;

			VulkanCommandPool m_ImmediateCmdPool;
			VkFence m_ImmediateFence;

			VulkanPipeline m_ComputePipeline;
			VkDescriptorSet m_ComputeDescriptorSet;
			VulkanPipeline m_GraphicsPipeline;
			VulkanPipeline m_RaytracePipeline;

			VkDescriptorPool m_ImGuiDescriptorPool;
			VkDescriptorSet m_ImGuiRenderImageDescriptor;
		};
	}
}