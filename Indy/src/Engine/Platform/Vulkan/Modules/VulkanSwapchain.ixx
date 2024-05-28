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

		struct VKSwapchainSupport
		{
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};

		struct VkSwapchainImage
		{
			VkImage image;
			VkImageView imageView;
		};

		class VulkanSwapchain
		{
		public:
			VulkanSwapchain(const std::shared_ptr<VulkanPhysicalDevice>& physicalDevice, const VkDevice& logicalDevice, const VkSurfaceKHR& surface, Window* window);
			~VulkanSwapchain();

			static void QuerySupportDetails(const std::shared_ptr<VulkanPhysicalDevice>& physicalDevice, const VkSurfaceKHR& surface);

		private:
			void ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
			void ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
			void ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

			void GetSwapchainImages();

		private:
			const VkDevice* m_LogicalDeviceHandle;
			VkSwapchainKHR m_Swapchain;
			VkSurfaceFormatKHR m_Format;
			VkPresentModeKHR m_PresentMode;
			VkExtent2D m_Extent;
			std::vector<VkSwapchainImage> m_Images;
		};
	}
}