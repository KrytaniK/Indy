#include "Context.h"

#include "Engine/Core/LogMacros.h"
#include "Engine/EventSystem/Events.h"

#include "DebugUtil.h"
#include "Device.h"
#include "SwapChain.h"
#include "Pipeline.h"
#include "CommandPool.h"
#include "DescriptorPool.h"
#include "Buffer.h"

/* TODO:
	- Viewport management is a problem. More than one 'viewport' has largely been implemented, but the handling for the creation/destruction is
		ambiguous at best. Implementation needs to be finished.

	- Mesh rendering needs to be implemented. This requires a system for submitting vertex, index and transform data, converting that	
		data into buffers, and mapping the correct data to the correct mesh during command buffer recording. The system should						<---[Current Target]---
		open up the internal API enough to where users can create arbitrary meshes procedurally using code through the exposed Renderer API.		

	- Finish out Vulkan Tutorial and implement remaining features
		- Texture Mapping
		- Depth Buffering
		- Model Loading (probably needs to happen externally, then have model data submitted to the renderer)
		- Generating Mipmaps
		- Multisampling
		- Compute Shaders

	- Look into Render subpasses
*/

namespace Engine::VulkanAPI
{
	VkInstance Context::s_Instance;
	std::vector<Viewport> Context::s_Viewports;

	Context::Context()
	{
		Events::Bind<Context>("Render", "Resize", this, &Context::onWindowResize);
	}

	Context::~Context()
	{
		Shutdown();
	}

	///////////////////////////////////////////////////
	// Vulkan Initialization //////////////////////////
	///////////////////////////////////////////////////

	void Context::Init(void* window)
	{
		///////////////////////////////////////////////////////
		// Debug Tool Initialization //////////////////////////

		DebugUtil::Init();

		//////////////////////////////////////////////////////
		// Vulkan Instance Creation //////////////////////////

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
		auto extensions = Context::GetRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (DebugUtil::Enabled())
		{
			createInfo.enabledLayerCount = DebugUtil::GetValidationLayerCount();
			createInfo.ppEnabledLayerNames = DebugUtil::GetValidationLayerNames();

			DebugUtil::PopulateMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		if (vkCreateInstance(&createInfo, nullptr, &s_Instance) != VK_SUCCESS) {
			INDY_CORE_ERROR("failed to create instance!");
		}

		//////////////////////////////////////////////////////
		// Debug Messanger Creation //////////////////////////

		DebugUtil::CreateMessenger(s_Instance);

		/////////////////////////////////////////////////////
		// Render Context Creation //////////////////////////

		if (s_Viewports.size() < 1)
			CreateContext(window);

		const VkDevice& logicalDevice = Device::GetLogicalDevice();

		////////////////////////////////////////////////////////////
		// Descriptor Pool Initialization //////////////////////////

		DescriptorPool::Init(logicalDevice, static_cast<uint32_t>(s_Viewports[0].frames.size()));

		//////////////////////////////////////////////////////////////////
		// Uniform Buffer Descriptor Set Layout //////////////////////////

		DescriptorPool::CreateUBODescriptorSetLayout(logicalDevice);

		////////////////////////////////////////////////////////////
		// Descriptor Sets Initialization //////////////////////////

		DescriptorPool::AllocateDescriptorSets(
			logicalDevice, s_Viewports[0], DescriptorPool::GetDescriptorSetLayout(UBO_DESCRIPTOR_SET_LAYOUT));

		DescriptorPool::UpdateDescriptorSets(logicalDevice, s_Viewports[0]);

		/////////////////////////////////////////////////////
		// Pipeline Initialization //////////////////////////

		Pipeline::Init(logicalDevice, s_Viewports[0]);

		//////////////////////////////////////////////////////////
		// Create Viewport Framebuffers //////////////////////////

		const VkRenderPass& renderPass = Pipeline::GetRenderPass();
		SwapChain::CreateFramebuffers(s_Viewports[0], renderPass);

		//////////////////////////////////////////////////////////
		// Create Viewport Sync Objects //////////////////////////

		SwapChain::CreateSyncObjects(s_Viewports[0]);

		/////////////////////////////////////////////////////////
		// Command Pool Initialization //////////////////////////

		const Queue& graphicsQueue = Device::GetGraphicsQueue();
		CommandPool::Init(logicalDevice, graphicsQueue);

		for (size_t i = 0; i < s_Viewports[0].frames.size(); i++)
		{
			CommandPool::CreateCommandBuffer(logicalDevice, s_Viewports[0].frames[i].commandBuffer);
		}
	}

	////////////////////////////////////////////
	// Vulkan Cleanup //////////////////////////
	////////////////////////////////////////////

	void Context::Shutdown()
	{
		const VkDevice& logicalDevice = Device::GetLogicalDevice();
		const VkCommandPool& commandPool = CommandPool::GetCommandPool();

		for (Viewport viewport : s_Viewports)
			viewport.Cleanup(logicalDevice, true);

		Pipeline::Shutdown(logicalDevice);
		CommandPool::Shutdown(logicalDevice);

		for (Viewport viewport : s_Viewports)
			viewport.FreeCommandBuffers(logicalDevice, commandPool);

		Device::Shutdown();
		DebugUtil::Shutdown(s_Instance);

		for (Viewport viewport : s_Viewports)
			vkDestroySurfaceKHR(s_Instance, viewport.surface, nullptr);
		
		vkDestroyInstance(s_Instance, nullptr);
	}

	////////////////////////////////////////////////////
	// Window Resize Callback //////////////////////////
	////////////////////////////////////////////////////

	void Context::onWindowResize(Event& event)
	{
		if (m_FramebufferResized) return;

		m_FramebufferResized = true;
	}

	///////////////////////////////////////////////////////
	// Context/Viewport Creation //////////////////////////
	///////////////////////////////////////////////////////

	void Context::CreateContext(void* window)
	{
		if (!s_Instance) 
		{
			Init(window);
			return;
		}

		uint32_t index = static_cast<uint32_t>(s_Viewports.size());

		Viewport viewport;
		viewport.window = window;

		if (glfwCreateWindowSurface(s_Instance, (GLFWwindow*)window, nullptr, &viewport.surface) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to create window surface!");
		}

		// If a window surface hasn't been created, a swap chain can't be created
		//	because the physical & logical devices haven't been selected/created.
		if (s_Viewports.size() < 1)
		{
			// Choose Physical Device, Create Logical Device, Create Device Queues.
			Device::Init(s_Instance, viewport);
		}

		SwapChain::CreateSwapchain(viewport);
		SwapChain::CreateImageViews(viewport);

		const VkPhysicalDeviceProperties& deviceProps = Device::GetPhysicalDeviceProperties();
		
		for (size_t i = 0; i < viewport.frames.size(); i++)
		{
			viewport.frames[i].uniformBuffers.view.buffer = std::make_shared<Buffer>(sizeof(ViewProjectionMatrix), 
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			viewport.frames[i].uniformBuffers.view.buffer->Map();
		}

		s_Viewports.resize(s_Viewports.size() + 1);
		s_Viewports[index] = viewport;
	}

	////////////////////////////////////////////////////
	// Extension Requirements //////////////////////////
	////////////////////////////////////////////////////

	std::vector<const char*> Context::GetRequiredExtensions()
	{
		// GLFW extensions are always required.
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (DebugUtil::Enabled()) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	/////////////////////////////////////////
	// Begin Frame //////////////////////////
	/////////////////////////////////////////

	void Context::BeginFrame()
	{
		const VkDevice& logicalDevice = Device::GetLogicalDevice();
		const VkRenderPass& renderPass = Pipeline::GetRenderPass();
		const VkPipeline& graphicsPipeline = Pipeline::GetPipeline();

		s_Viewports[0].Wait(logicalDevice);

		VkResult result = s_Viewports[0].GetNextImage(logicalDevice);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			SwapChain::RefreshSwapchain(s_Viewports[0], renderPass);
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to Acquire Swap Chain Image!");
		}

		// Reset the current fence
		s_Viewports[0].ResetFence(logicalDevice);

		// Reset the current command buffer
		s_Viewports[0].ResetCommandBuffer();

		// Start recording commands and initialize the render pass
		CommandPool::RecordCommandBuffer_Begin(s_Viewports[0], renderPass, graphicsPipeline);
	}

	///////////////////////////////////////
	// End Frame //////////////////////////
	///////////////////////////////////////

	void Context::EndFrame(Camera& camera)
	{
		const VkPipelineLayout& pipelineLayout = Pipeline::GetLayout();

		CommandPool::RecordCommandBuffer_End(s_Viewports[0], camera, pipelineLayout);
	}

	////////////////////////////////////////
	// Draw Frame //////////////////////////
	////////////////////////////////////////

	void Context::DrawFrame()
	{
		const VkDevice& logicalDevice = Device::GetLogicalDevice();
		const Queue& graphicsQueue = Device::GetGraphicsQueue();
		const Queue& presentQueue = Device::GetPresentQueue();

		const VkRenderPass& renderPass = Pipeline::GetRenderPass();

		Frame currentFrame = s_Viewports[0].GetCurrentFrame();

		// Submit the command buffer to the correct queue
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		// Specify when and where to wait in the graphics pipeline
		VkSemaphore waitSemaphores[] = { currentFrame.imageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		// attach the command buffer to the submission
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &currentFrame.commandBuffer;

		// Specify which semaphores to signal when the command buffers finish execution
		VkSemaphore signalSemaphores[] = { currentFrame.renderFinishedSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (submitInfo.pCommandBuffers == nullptr)
			INDY_CORE_CRITICAL("Command Buffer Reference(s) are null!");

		// Submit commands to the graphics queue.
		if (vkQueueSubmit(graphicsQueue.queue, 1, &submitInfo, currentFrame.fence) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Failed to submit draw command buffer!");
			return;
		}

		// Push results back to the swap chain for presentation
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { s_Viewports[0].swapchain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &s_Viewports[0].imageIndex;
		presentInfo.pResults = nullptr; // Optional

		// Submit presentation information to the present queue
		VkResult result = vkQueuePresentKHR(presentQueue.queue, &presentInfo);

		// Recreate Swap Chain if it is out of date, or suboptimal (or the window was resized)
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_FramebufferResized)
		{
			m_FramebufferResized = false;

			// Window Minimization Handling
			int width = 0, height = 0;
			GLFWwindow* window = (GLFWwindow*)s_Viewports[0].window;
			glfwGetFramebufferSize(window, &width, &height);

			// There are cases where the window height will be zero but the window won't be minimized.
			//	There might be a query for minimization, but checking for both width and height to be 0
			//	works well enough.
			while (width == 0 && height == 0)
			{
				glfwGetFramebufferSize(window, &width, &height);
				glfwWaitEvents();
			}

			SwapChain::RefreshSwapchain(s_Viewports[0], renderPass);
		}
		else if (result != VK_SUCCESS)
		{
			INDY_CORE_CRITICAL("Failed to Acquire Swap Chain Image!");
		}

		s_Viewports[0].Wait(logicalDevice);
		CommandPool::FlushDrawQueue();
	}

	//////////////////////////////////////////
	// Swap Buffers //////////////////////////
	//////////////////////////////////////////

	void Context::SwapBuffers()
	{
		s_Viewports[0].SwapBuffers();
	}

	/////////////////////////////////////////////
	// Draw Submission //////////////////////////
	/////////////////////////////////////////////

	void Context::Submit(void* vertices, uint32_t vertexCount, void* indices, uint32_t indexCount, uint32_t instanceCount)
	{
		CommandPool::SubmitDrawCall(vertices, vertexCount, indices, indexCount, instanceCount);
	}
}

	
