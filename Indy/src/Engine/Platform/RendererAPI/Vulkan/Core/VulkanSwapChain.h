#pragma once

#include "VulkanDevice.h"

namespace Engine
{
	class VulkanSwapChain
	{
	public:
		void Init(Engine::VulkanDevice::SwapChainSupportDetails supportDetails, VkSurfaceKHR windowSurface);
		void Shutdown();

	private:
		VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

		VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

		VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	private:
		VulkanDevice::SwapChainSupportDetails m_SupportDetails;

		VkSwapchainKHR m_SwapChain;

		// Swap Chain Images
		std::vector<VkImage> m_Images;

		// Swap Chain Image Format
		VkFormat m_ImageFormat;

		// Swap Chain Extent
		VkExtent2D m_Extent;
	};
}