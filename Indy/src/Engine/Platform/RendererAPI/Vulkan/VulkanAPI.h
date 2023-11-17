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
		static uint32_t MAX_FRAMES_IN_FLIGHT;
		static VkInstance s_Vulkan_Instance;
		static void* s_Window;

	public:
		VulkanAPI();
		~VulkanAPI();

	public:
		virtual void Init() override;
		virtual void Shutdown() override;

		virtual void DrawFrame() override;

		virtual void onWindowResize(Event& event) override;

	private:
		
		// Instance Creation
		void CreateInstance();
		std::vector<const char*> GetRequiredExtensions();

		// Window Surface Creation
		void CreateWindowSurface();

		// Sync Objects (Temp)
		void CreateSyncObjects();

		// Swap Chain Recreation
		void RecreateSwapChain();

	private:
		struct VulkanFrame
		{
			VkSemaphore imageAvailableSemaphore;
			VkSemaphore renderFinishedSemaphore;
			VkFence fence;
		};

	private:
		// Window Surface
		VkSurfaceKHR m_WindowSurface;

		std::vector<VulkanFrame> m_FramesInFlight;
		uint32_t m_CurrentFrame = 0;
		bool m_FramebufferResized = false;
	};
}