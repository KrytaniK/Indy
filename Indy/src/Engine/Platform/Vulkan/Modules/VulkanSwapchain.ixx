module;

#include <memory>
#include <vector>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Swapchain;

/* Note:
 *	The swapchain's settings should change based on the target build device.
 *	Since I'm only currently targeting desktop devices with this game engine,
 *	I'm opting for the best performance/quality tradeoffs.
 */

import Indy.Window;

export
{
	namespace Indy
	{
		struct VulkanPhysicalDevice;

		struct VulkanSwapchainSupport
		{
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};

		struct VulkanSwapchainImage
		{
			VkImage image = VK_NULL_HANDLE;
			VkImageView imageView = VK_NULL_HANDLE;
		};

		class VulkanSwapchain
		{
		public:
			VulkanSwapchain(const VulkanPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const VkSurfaceKHR& surface, Window* window);
			~VulkanSwapchain();

			static void QuerySupportDetails(VulkanPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);

			VkSwapchainKHR& Get() { return m_Swapchain; };
			const VkExtent2D& GetExtent() { return m_Extent; };
			const VkSurfaceFormatKHR& GetFormat() { return m_Format; };
			VulkanSwapchainImage& GetImage(const uint32_t& index);

		private:
			void ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
			void ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
			void ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

			void GetSwapchainImages();

		private:
			VkDevice m_LogicalDevice;
			VkSwapchainKHR m_Swapchain;
			VkSurfaceFormatKHR m_Format;
			VkPresentModeKHR m_PresentMode;
			VkExtent2D m_Extent;
			std::vector<VulkanSwapchainImage> m_Images;
		};
	}
}