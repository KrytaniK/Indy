#pragma once

#include "../RendererAPI.h"

#include <vulkan/vulkan.h>

namespace Engine
{
	class VulkanAPI : public RendererAPI
	{
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

		// Debug
		bool CheckValidationLayerSupport();
		void SetupDebugMessenger();
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		// Physical Device Creation
		void PickPhysicalDevice();
		bool IsDeviceSuitable(VkPhysicalDevice device);

		struct QueueFamilyIndices
		{
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentFamily;

			bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); };
		};

		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

		// Logical Device Creation
		void CreateLogicalDevice();

		// Window Surface Creation
		void CreateWindowSurface();

	private:
		// Instance
		VkInstance m_Vulkan_Instance;

		// Debug
		VkDebugUtilsMessengerEXT m_DebugMessenger;

		// Physical Device
		VkPhysicalDevice m_PhysicalDevice;

		// Logical Device
		VkDevice m_LogicalDevice;

		// Graphics Queue (For Graphics Operations)
		VkQueue m_GraphicsQueue;

		// Presentation Queue (For Presenting graphics to the window)
		VkQueue m_PresentQueue;

		// Window Surface (aka, the Render Context)
		VkSurfaceKHR m_WindowSurface;


		const std::vector<const char*> m_ValidationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		#ifdef ENGINE_DEBUG
			const bool m_EnableValidationLayers = true;
		#else
			const bool m_EnableValidationLayers = false;
		#endif
	};
}