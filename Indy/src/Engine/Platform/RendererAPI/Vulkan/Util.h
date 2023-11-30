#pragma once

#include "Engine/Core/Log.h"
#include "Buffer.h"

#include <optional>
#include <vector>
#include <array>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace Engine::VulkanAPI
{
	/////////////////////////////////////////
	// Constants ////////////////////////////
	/////////////////////////////////////////

	const std::vector<const char*> c_DeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	const std::vector<const char*> c_ValidationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

	//////////////////////////////////////////
	// Containers ////////////////////////////
	//////////////////////////////////////////

	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 color;

		static VkVertexInputBindingDescription GetBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription;
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
		{
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

			// Position Description
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			// Color Description
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			return attributeDescriptions;
		}
	};

	struct UniformBufferObject {
		alignas(16) glm::mat4 model; // alignas(16) enforces alignment and ensures issues don't arise between C++ and shaders
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

	struct Frame
	{
		VkSemaphore imageAvailableSemaphore;
		VkSemaphore renderFinishedSemaphore;
		VkFence fence;
		VkCommandBuffer commandBuffer;

		std::vector<Buffer> vertexBuffers;

		std::shared_ptr<Buffer> uniformBuffer; // Split uniform buffers into two (view/proj and model). Model UBO is dynamic, used for each mesh object to be rendered. view/proj is used for camera.
	};

	struct SwapChainSupport
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct Viewport
	{
		void* window;
		VkSurfaceKHR surface;
		VkSwapchainKHR swapchain;
		SwapChainSupport swapchainSupport;
		VkFormat imageFormat;
		VkExtent2D extent;

		std::vector<Frame> frames = std::vector<Frame>(MAX_FRAMES_IN_FLIGHT);
		std::vector<VkImage> swapchainImages;
		std::vector<VkImageView> swapchainImageViews;
		std::vector<VkFramebuffer> framebuffers;
		std::vector<VkDescriptorSet> descriptorSets;
		uint32_t currentFrame = 0;
		uint32_t imageIndex;

		void Cleanup(const VkDevice& logicalDevice, const bool destroySwapChain)
		{
			for (size_t i = 0; i < swapchainImages.size(); i++)
			{
				vkDestroyFramebuffer(logicalDevice, framebuffers[i], nullptr);
				vkDestroyImageView(logicalDevice, swapchainImageViews[i], nullptr);
			}

			if (!destroySwapChain) return;

			for (size_t i = 0; i < frames.size(); i++)
			{
				vkDestroySemaphore(logicalDevice, frames[i].renderFinishedSemaphore, nullptr);
				vkDestroySemaphore(logicalDevice, frames[i].imageAvailableSemaphore, nullptr);
				vkDestroyFence(logicalDevice, frames[i].fence, nullptr);
			}

			vkDestroySwapchainKHR(logicalDevice, swapchain, nullptr);
		}

		void FreeCommandBuffers(const VkDevice& logicalDevice, const VkCommandPool& commandPool)
		{
			for (auto& frame : frames)
			{
				vkFreeCommandBuffers(logicalDevice, commandPool, 1, &frame.commandBuffer);
			}
		}

		void Wait(const VkDevice& logicalDevice)
		{
			vkWaitForFences(logicalDevice, 1, &frames[currentFrame].fence, VK_TRUE, UINT64_MAX);
		}

		VkResult GetNextImage(const VkDevice& logicalDevice)
		{
			return vkAcquireNextImageKHR(logicalDevice, swapchain, UINT64_MAX,
				frames[currentFrame].imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
		}

		Frame GetCurrentFrame() const { return frames[currentFrame]; };

		void ResetFence(const VkDevice& logicalDevice)
		{
			vkResetFences(logicalDevice, 1, &frames[currentFrame].fence);
		}

		void ResetCommandBuffer()
		{
			vkResetCommandBuffer(frames[currentFrame].commandBuffer, 0);
		}

		void SwapBuffers()
		{
			currentFrame = (currentFrame + 1) % frames.size();
		}
	};

	struct Queue
	{
		VkQueue queue;
		uint32_t familyIndex;
	};

	struct QueueFamilyIndex
	{
		std::optional<uint32_t> index;

		bool IsValid() { return index.has_value(); };
		uint32_t Value() { return index.value(); };
		void Set(uint32_t value) { index = value; };
	};

	///////////////////////////////////////////////////////
	// Debug Utility Functions ////////////////////////////
	///////////////////////////////////////////////////////

	static VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan_DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	) {

		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			INDY_CORE_ERROR("[Vulkan Debug] {0}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			INDY_CORE_WARN("[Vulkan Debug] {0}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			INDY_CORE_INFO("[Vulkan Debug] {0}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			INDY_CORE_TRACE("[Vulkan Debug] {0}", pCallbackData->pMessage);
			break;
		default:
			INDY_CORE_INFO("[Vulkan Debug] {0}", pCallbackData->pMessage);
		}

		return VK_FALSE;
	}

	static VkResult Vulkan_CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	static void Vulkan_DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}
}