#include "VulkanAPI.h"

#include "Engine/Core/Log.h"
#include <set>

#include <GLFW/glfw3.h>

namespace Engine
{
	static VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan_DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	) {

		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			INDY_CORE_ERROR("[VulkanAPI] {0}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			INDY_CORE_WARN("[VulkanAPI] {0}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			INDY_CORE_INFO("[VulkanAPI] {0}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			INDY_CORE_TRACE("[VulkanAPI] {0}", pCallbackData->pMessage);
			break;
		default:
			INDY_CORE_INFO("[VulkanAPI] {0}", pCallbackData->pMessage);
		}

		return VK_FALSE;
	}

	/*
		There are a couple of functions that Vulkan doesn't load automatically.
		Because of this, when using the DebugMessenger, we need to load them
		manually.
	*/
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

	VulkanAPI::VulkanAPI() : 
		m_Vulkan_Instance(nullptr), m_DebugMessenger(nullptr), 
		m_PhysicalDevice(nullptr), m_LogicalDevice(nullptr), 
		m_WindowSurface(nullptr), m_GraphicsQueue(nullptr),
		m_PresentQueue(nullptr)
	{

	}

	VulkanAPI::~VulkanAPI()
	{
		Shutdown();
	}

	void VulkanAPI::Init()
	{
		if (m_EnableValidationLayers && !CheckValidationLayerSupport())
		{
			INDY_CORE_ERROR("[Vulkan] Validation layers requested, but one or more are not available!");
		}

		CreateInstance();
		SetupDebugMessenger(); // Requires the vulkan instance to be created.
		CreateWindowSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();
	}

	void VulkanAPI::Shutdown()
	{
		vkDestroyDevice(m_LogicalDevice, nullptr);

		if (m_EnableValidationLayers)
		{
			// Must be called before the vulkan instance is destroyed.
			Vulkan_DestroyDebugUtilsMessengerEXT(m_Vulkan_Instance, m_DebugMessenger, nullptr);
		}

		vkDestroySurfaceKHR(m_Vulkan_Instance, m_WindowSurface, nullptr);
		vkDestroyInstance(m_Vulkan_Instance, nullptr);
	}

	void VulkanAPI::onApplicationClose(Event& event)
	{
		Shutdown();
	}

	// ------------------------
	// Vulkan Instance Creation
	// ------------------------

	void VulkanAPI::CreateInstance()
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
		auto extensions = GetRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		/* The debug messenger can't debug the creation or destruction of the vulkan instance by default. 
			To do so, a pointer to the debug messenger create info struct is needed in the create info for the vulkan instance. 
			This creates an additional DebugMessenger for the two methods responsible for the creation
			and destroying of the VkInstance.

		*/
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (m_EnableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
			createInfo.ppEnabledLayerNames = m_ValidationLayers.data();

			PopulateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else
		{
			createInfo.enabledLayerCount = 0;

			createInfo.pNext = nullptr;
		}

		// global validation layers
		createInfo.enabledLayerCount = 0;

		if (vkCreateInstance(&createInfo, nullptr, &m_Vulkan_Instance) != VK_SUCCESS) {
			INDY_CORE_ERROR("failed to create instance!");
		}
	}

	// -------------------------------------------
	// Validation Layers and Debug Messenger Setup
	// -------------------------------------------

	bool VulkanAPI::CheckValidationLayerSupport()
	{
		// Request the number of layers
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		// Query for layer details
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		// Ensure all layers specified in m_ValidationLayers actually exist
		for (const char* layerName : m_ValidationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}

		return true;
	}

	std::vector<const char*> VulkanAPI::GetRequiredExtensions()
	{
		// GLFW extensions are always required.
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (m_EnableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	void VulkanAPI::SetupDebugMessenger()
	{
		if (!m_EnableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		PopulateDebugMessengerCreateInfo(createInfo);

		if (Vulkan_CreateDebugUtilsMessengerEXT(m_Vulkan_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("[Vulkan] Failed to set up Debug Messenger!");
		}
	}

	void VulkanAPI::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = Vulkan_DebugCallback;
		createInfo.pUserData = nullptr;
	}

	// ------------------------
	// Physical Device Creation
	// ------------------------

	// Physical Device is automatically cleaned up.
	void VulkanAPI::PickPhysicalDevice()
	{
		// Query for device count
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_Vulkan_Instance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			INDY_CORE_ERROR("Failed to find GPUs with Vulkan Support!");
		}

		// Get all GPUs that support Vulkan
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_Vulkan_Instance, &deviceCount, devices.data());

		for (const auto& device : devices)
		{
			if (IsDeviceSuitable(device))
			{
				m_PhysicalDevice = device;
				break;
			}
		}

		if (m_PhysicalDevice == VK_NULL_HANDLE)
		{
			INDY_CORE_ERROR("Failed to find a suitable GPU!");
		}
	}

	/* This can be as simple or as complex as needed.I can query for
		things such as geometry shader support. I can rank all GPUs by
		some score and choose the one that has the highest score, which
		could be based on the max image size. (See Vulkan Examples)

		For now, Vulkan support is the only thing needed.
	*/
	bool VulkanAPI::IsDeviceSuitable(VkPhysicalDevice device)
	{
		// Query for device properties
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		// Query for device features
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		// Ensure the device can process graphics commands 
		// (this is implied in FindQueueFamilies)
		QueueFamilyIndices indices = FindQueueFamilies(device);
		return indices.isComplete();
	}

	// This can be thought of as an extension to IsDeviceSuitable, though its
	//	intention is not for selecting the device. It's more to find the queue
	//	families that you need for your application that happen to be tied to
	//	that device.
	VulkanAPI::QueueFamilyIndices VulkanAPI::FindQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices;

		// Query for queue family count
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		// Retrieve queue family data
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			VkBool32 presentSupport = VK_FALSE;
			// Ensure there is a queue dedicated to presentation (Can be the same as the graphics queue family in some cases)
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_WindowSurface, &presentSupport);

			if (presentSupport)
				indices.presentFamily = i;

			// Ensure there is a queue dedicated to graphics operations
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.graphicsFamily = i;

			if (indices.isComplete())
				break;

			i++;
		}

		return indices;
	}

	// -----------------------
	// Logical Device Creation
	// -----------------------

	void VulkanAPI::CreateLogicalDevice()
	{
		QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);

		// Create queues for graphics and present queues.
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		// This is where you would specify the GPU features you want to use.
		VkPhysicalDeviceFeatures deviceFeatures{};

		// Creating the Logical Device
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;

		if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice) != VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
		}

		// Create the Graphics Queue Family Handle
		vkGetDeviceQueue(m_LogicalDevice, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);

		// Create the Presentation Queue Family Handle
		vkGetDeviceQueue(m_LogicalDevice, indices.presentFamily.value(), 0, &m_PresentQueue);
	}

	// -----------------------
	// Window Surface Creation
	// -----------------------

	void VulkanAPI::CreateWindowSurface()
	{
		Event event{"LayerContext", "RequestWindow"};
		Events::Dispatch(event);
		GLFWwindow* GLFW_Window = (GLFWwindow*)event.data;

		if (glfwCreateWindowSurface(m_Vulkan_Instance, GLFW_Window, nullptr, &m_WindowSurface) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create window surface!");
		}
	}
}