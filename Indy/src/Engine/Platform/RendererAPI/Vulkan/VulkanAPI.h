#pragma once

#include "../RendererAPI.h"

#include "VulkanDebugUtil.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanPipeline.h"
#include "VulkanCommandPool.h"

#include <vulkan/vulkan.h>

namespace Engine
{
	class VulkanAPI : public RendererAPI
	{
	private:
		static VkInstance s_Vulkan_Instance;

	public:
		VulkanAPI();
		~VulkanAPI();

	public:
		virtual void Init() override;
		virtual void Shutdown() override;

	private:
		void onApplicationClose(Event& event);
		
		// Instance Creation
		void CreateInstance();
		std::vector<const char*> GetRequiredExtensions();

		// Window Surface Creation
		void CreateWindowSurface();
		
		// Framebuffer Creation
		void CreateFramebuffers();

		// Sync Objects (Temp)
		void CreateSyncObjects();

		// Temp Draw Method
		void DrawFrame();

	private:
		// Instance
		VkInstance m_Vulkan_Instance;

		// Window Surface
		VkSurfaceKHR m_WindowSurface;

		// Frame Buffers
		std::vector<VkFramebuffer> m_Framebuffers;

		// Debug Util
		std::unique_ptr<VulkanDebugUtil> m_DebugUtil;

		// Physical & Logical Devices & Relative Queue Families
		std::unique_ptr<VulkanDevice> m_Device;

		// Swap Chain
		std::unique_ptr<VulkanSwapChain> m_SwapChain;
		
		// Graphics Pipeline
		std::unique_ptr<VulkanPipeline> m_Pipeline;

		// Command Bool & Buffers
		std::unique_ptr<VulkanCommandPool> m_CommandPool;

		// Temp rendering & presentation (semaphores & fences)
		VkSemaphore m_ImageAvailableSemaphore;
		VkSemaphore m_RenderFinishedSemaphore;
		VkFence m_InFlightFence;
	};
}