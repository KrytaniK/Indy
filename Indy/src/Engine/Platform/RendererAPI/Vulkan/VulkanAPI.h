#pragma once

#include "../RendererAPI.h"

#include "Core/VulkanDebugUtil.h"
#include "Core/VulkanDevice.h"
#include "Core/VulkanSwapChain.h"

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

	private:
		// Instance
		VkInstance m_Vulkan_Instance;

		// Window Surface
		VkSurfaceKHR m_WindowSurface;

		// Debug Util
		std::unique_ptr<VulkanDebugUtil> m_DebugUtil;

		// Physical & Logical Devices & Relative Queue Families
		std::unique_ptr<VulkanDevice> m_Device;

		// Swap Chain
		std::unique_ptr<VulkanSwapChain> m_SwapChain;
	};
}