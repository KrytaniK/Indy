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
			VulkanRenderer(Window* window, const VkInstance& instance, const VkSurfaceKHR& surface, const std::shared_ptr<VulkanDevice>& device);
			virtual ~VulkanRenderer() override;

			virtual void Render() override;
			
			virtual void Enable() override;
			virtual void Disable() override;

		private:
			void BuildPipelines();
			void InitImGui();
			void RenderImGui(const VkCommandBuffer& cmdBuffer, const VkImageView& imageView);

		private:
			Window* m_Window;
			VkInstance m_Instance;
			VkSurfaceKHR m_Surface;
			VmaAllocator m_ImageAllocator;

			std::shared_ptr<VulkanDevice> m_Device;

			VulkanSwapchain m_Swapchain;
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

			VulkanRendererPipelineInfo m_Pipelines;

			VkDescriptorPool m_ImGuiDescriptorPool;
			VkDescriptorSet m_ImGuiRenderImageDescriptor;
		};
	}
}