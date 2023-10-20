#include "VulkanAPI.h"

#include "Engine/Core/Log.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	VkInstance VulkanAPI::s_Vulkan_Instance = nullptr;

	VulkanAPI::VulkanAPI()
	{
		Events::Bind<VulkanAPI>("LayerContext", "AppClose", this, &VulkanAPI::onApplicationClose);
	}

	VulkanAPI::~VulkanAPI()
	{
		this->Shutdown();
	}

	void VulkanAPI::Init()
	{
		// App Info
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Indy Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		// Instance Creation Info
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		// Getting Platform Specific Extensions
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;

		// global validation layers
		createInfo.enabledLayerCount = 0;

		VkResult result = vkCreateInstance(&createInfo, nullptr, &s_Vulkan_Instance);

		if (result != VK_SUCCESS) {
			INDY_CORE_ERROR("failed to create instance!");
		}

		INDY_CORE_TRACE("Vulkan Initialized");
	}

	void VulkanAPI::Shutdown()
	{
		vkDestroyInstance(s_Vulkan_Instance, nullptr);
	}

	void VulkanAPI::onApplicationClose(Event& event)
	{
		this->Shutdown();
	}
}