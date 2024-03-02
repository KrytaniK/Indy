#pragma once

#include "Util.h"

#include <GLFW\glfw3.h>

namespace Engine::VulkanAPI
{
	class SwapChain
	{
	public:
		// Creates a swap chain and image views for a given render context
		static void CreateSwapchain(Viewport& viewport, bool isRefreshed = false);
		static void RefreshSwapchain(Viewport& viewport, const VkRenderPass& renderPass);
			
		static void CreateImageViews(Viewport& viewport);
		static void CreateFramebuffers(Viewport& viewport, const VkRenderPass& renderPass);
		static void CreateSyncObjects(Viewport& viewport);

	private:
		static VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		static VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		static VkExtent2D ChooseExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);
	};
}