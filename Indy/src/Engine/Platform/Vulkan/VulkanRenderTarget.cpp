#include "Engine/Core/LogMacros.h"

#include <memory>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "imgui_internal.h"

import Indy.Profiler;
import Indy.Graphics;
import Indy.VulkanGraphics;
import Indy.Events;

namespace Indy
{
	VulkanRenderTarget::VulkanRenderTarget(const VkInstance& instance, const VulkanRTSpec& spec)
	{
		m_ID = spec.window->Properties().id;
		m_Instance = instance;
		m_DeviceHandle = spec.deviceHandle;
		m_ComputePipeline = spec.computePipeline;
		m_FrameCount = 0;

		if (!spec.useSurface)
			return;

		// Create VkSurface
		if (glfwCreateWindowSurface(m_Instance, static_cast<GLFWwindow*>(spec.window->NativeWindow()), nullptr, &m_Surface) != VK_SUCCESS)
		{
			INDY_CORE_CRITICAL("Failed to create window surface!");
			return;
		}

		auto physicalDevice = m_DeviceHandle->GetPhysicalDevice();
		if (!VulkanDevice::GetGPUSurfaceSupport(physicalDevice, m_Surface))
		{
			INDY_CORE_ERROR("Failed to create render target. GPU not support surface presentation!");
			return;
		}

		// Create Swapchain with window
		m_Swapchain = std::make_unique<VulkanSwapchain>(physicalDevice, m_DeviceHandle->Get(), m_Surface, spec.window);

		VulkanImageSpec offscreenImageSpec{};
		offscreenImageSpec.extent = {
			spec.window->Properties().width,
			spec.window->Properties().height,
			1
		};
		offscreenImageSpec.format = VK_FORMAT_R16G16B16A16_SFLOAT;
		offscreenImageSpec.usageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		offscreenImageSpec.usageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		offscreenImageSpec.usageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
		offscreenImageSpec.usageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		offscreenImageSpec.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;

		m_OffscreenImage = std::make_unique<VulkanImage>(m_DeviceHandle->GetVmaAllocator(), m_DeviceHandle->Get(), offscreenImageSpec);

		m_ImmediateCommandPool = std::make_unique<VulkanCommandPool>(
			m_DeviceHandle->Get(), 
			physicalDevice->queueFamilies.graphics.value(), 
			1
		);
		m_ImmediateSyncObjects = std::make_unique<VulkanSyncObjects>(m_DeviceHandle->Get());
		m_ImmediateSyncObjects->AddFence("Immediate Submit", VK_FENCE_CREATE_SIGNALED_BIT);

		// Allocate frame data for this render target
		m_Frames.reserve(g_Max_Frames_In_Flight);
		for (uint8_t i = 0; i < g_Max_Frames_In_Flight; i++)
			m_Frames.emplace_back(std::make_unique<VulkanFrame>(m_DeviceHandle, physicalDevice->queueFamilies));

		// Offscreen Image Descriptor Info
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imageInfo.imageView = m_OffscreenImage->GetView();

		// Update compute image descriptor
		auto computeDescriptor = m_ComputePipeline->GetDescriptor(INDY_SHADER_TYPE_COMPUTE);
		computeDescriptor->UpdateImageBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0, &imageInfo);
		computeDescriptor->UpdateDescriptorSets(m_DeviceHandle->Get());

		// TEMP: MOVE LATER!! -----------------
			init_imgui(instance, spec.window);
		// ------------------------------------
	}

	VulkanRenderTarget::~VulkanRenderTarget()
	{
		// Wait for GPU to finish up any tasks
		vkDeviceWaitIdle(m_DeviceHandle->Get());

		// Explicitly delete all frames
		for (int i = 0; i < m_Frames.size(); i++)
			m_Frames[i] = nullptr;

		// Explicitly destroy offscreen image
		m_OffscreenImage = nullptr;

		// Explicitly destroy swap chain.
		m_Swapchain = nullptr; 

		// Explicitly destroy surface
		if (&m_Surface)
			vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);

		// TEMP
		ImGui_ImplVulkan_Shutdown();
		vkDestroyDescriptorPool(m_DeviceHandle->Get(), m_ImGuiPool, nullptr);
	}

	void VulkanRenderTarget::Render()
	{
		// TEMP IMGUI
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::ShowDemoWindow();

		ImGui::Render();

		// ----------

		// Get Current Frame
		FrameData frameData = GetCurrentFrameData();

		// Dispatch Compute Operations
		OnBeginFrame(frameData);

		// Execute Graphics Commands
		OnDrawFrame(frameData);

		// Present Frame
		OnPresentFrame(frameData);

		m_FrameCount++;
	}

	void VulkanRenderTarget::OnBeginFrame(const FrameData& frameData)
	{
		// Wait for GPU to finish compute operations
		if (vkWaitForFences(m_DeviceHandle->Get(), 1, &frameData.computeFence, VK_TRUE, UINT64_MAX) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Error waiting for Compute Fence");
			return;
		}

		// Update compute descriptor data (if needed)
		// ----------------------------

		// ----------------------------

		// Reset compute fence
		if (vkResetFences(m_DeviceHandle->Get(), 1, &frameData.computeFence) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Error resetting Render Fence");
			return;
		}

		// Begin compute command buffer
		frameData.computeCommandPool->BeginCommandBuffer(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		// Transition offscreen image from undefined to general layout (for writing)
		VulkanImage::TransitionLayout(
			frameData.computeCommandBuffer, m_OffscreenImage->Get(),
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL,
			VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
			0, VK_ACCESS_2_MEMORY_WRITE_BIT
		);

		{ // Execute compute commands

			// Bind compute pipeline
			vkCmdBindPipeline(frameData.computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_ComputePipeline->Get());

			// Bind compute render image descriptors
			vkCmdBindDescriptorSets(frameData.computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_ComputePipeline->GetLayout(), 0, 1,&m_ComputePipeline->GetDescriptor(INDY_SHADER_TYPE_COMPUTE)->GetSet(), 0, nullptr);

			vkCmdDispatch(
				frameData.computeCommandBuffer,
				std::ceil(m_OffscreenImage->GetExtent().width / 16.0),
				std::ceil(m_OffscreenImage->GetExtent().height / 16.0),
				1
			);
		}

		frameData.computeCommandPool->EndCommandBuffer(0);

		SubmitComputeQueue(frameData);
	}

	void VulkanRenderTarget::OnDrawFrame(FrameData& frameData)
	{
		// Wait for GPU to finish render operations
		if (vkWaitForFences(m_DeviceHandle->Get(), 1, &frameData.renderFence, VK_TRUE, UINT64_MAX) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Error waiting for Render Fence");
			return;
		}

		// Update per-frame descriptors
		// ----------------------------

		// ----------------------------

		// Reset the current fence
		if (vkResetFences(m_DeviceHandle->Get(), 1, &frameData.renderFence) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Error resetting Render Fence");
			return;
		}

		// Obtain the next swapchain image (TODO: only if we're rendering to a surface) 
		if (vkAcquireNextImageKHR(m_DeviceHandle->Get(), m_Swapchain->Get(), UINT64_MAX, frameData.imageAvailableSemaphore, nullptr, &frameData.swapchainImageIndex) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Error retrieving next swapchain image!");
			return;
		}

		// Begin the graphics command buffer
		frameData.graphicsCommandPool->BeginCommandBuffer(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		{ // Graphics Commands
			
		}

		// Transition rendered image from general use (writing) to transfer source
		VulkanImage::TransitionLayout(
			frameData.graphicsCommandBuffer, m_OffscreenImage->Get(),
			VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
			VK_ACCESS_2_MEMORY_WRITE_BIT, VK_ACCESS_2_MEMORY_READ_BIT
		);

		// Transition swap chain image from undefined to transfer destination (for copying)
		VulkanImage::TransitionLayout(
			frameData.graphicsCommandBuffer, m_Swapchain->GetImage(frameData.swapchainImageIndex).image,
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
			0, VK_ACCESS_2_MEMORY_WRITE_BIT
		);

		// Copy rendered image to swap chain image
		VulkanImage::Copy(
			frameData.graphicsCommandBuffer, m_OffscreenImage->Get(), m_Swapchain->GetImage(frameData.swapchainImageIndex).image,
			{ m_OffscreenImage->GetExtent().width, m_OffscreenImage->GetExtent().height }, m_Swapchain->GetExtent()
		);

		// Transition swap chain image from transfer destination to color attachment optimal, so it can receive commands
		VulkanImage::TransitionLayout(
			frameData.graphicsCommandBuffer, m_Swapchain->GetImage(frameData.swapchainImageIndex).image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
			VK_ACCESS_2_MEMORY_WRITE_BIT, VK_ACCESS_2_MEMORY_WRITE_BIT
		);

		{ // Post-graphics commands (for ImGui)

			draw_imgui(frameData.graphicsCommandBuffer, m_Swapchain->GetImage(frameData.swapchainImageIndex).imageView);

		}

		// Transition swap chain image from transfer destination to present
		VulkanImage::TransitionLayout(
			frameData.graphicsCommandBuffer, m_Swapchain->GetImage(frameData.swapchainImageIndex).image,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
			VK_ACCESS_2_MEMORY_WRITE_BIT, VK_ACCESS_2_MEMORY_READ_BIT
		);

		frameData.graphicsCommandPool->EndCommandBuffer(0);

		SubmitGraphicsQueue(frameData);
	}

	void VulkanRenderTarget::OnPresentFrame(const FrameData& frameData)
	{
		// Present image
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.pSwapchains = &m_Swapchain->Get();
		presentInfo.swapchainCount = 1;

		// Attach wait semaphore
		presentInfo.pWaitSemaphores = &frameData.renderSemaphore;
		presentInfo.waitSemaphoreCount = 1;

		// Attach swap chain image index
		presentInfo.pImageIndices = &frameData.swapchainImageIndex;

		if (vkQueuePresentKHR(frameData.graphicsQueue, &presentInfo) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Error Submitting Graphics Queue!");
			return;
		}
	}

	void VulkanRenderTarget::SubmitComputeQueue(const FrameData& frameData)
	{
		// Submit compute command buffer
		VkCommandBufferSubmitInfo computeBufferInfo{};
		computeBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
		computeBufferInfo.commandBuffer = frameData.computeCommandBuffer;
		computeBufferInfo.deviceMask = 0;
		computeBufferInfo.pNext = nullptr;

		// Submit compute semaphore
		VkSemaphoreSubmitInfo computeSemaphoreInfo{};
		computeSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		computeSemaphoreInfo.pNext = nullptr;
		computeSemaphoreInfo.semaphore = frameData.computeSemaphore;
		computeSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
		computeSemaphoreInfo.deviceIndex = 0;
		computeSemaphoreInfo.value = 1;

		VkSubmitInfo2 computeSubmitInfo{};
		computeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;

		// Attach compute command buffer
		computeSubmitInfo.commandBufferInfoCount = 1;
		computeSubmitInfo.pCommandBufferInfos = &computeBufferInfo;

		// Attach compute command semaphore
		computeSubmitInfo.signalSemaphoreInfoCount = 1;
		computeSubmitInfo.pSignalSemaphoreInfos = &computeSemaphoreInfo;

		// Submit compute operations
		if (vkQueueSubmit2(frameData.computeQueue, 1, &computeSubmitInfo, frameData.computeFence) != VK_SUCCESS)
		{
			INDY_CORE_CRITICAL("Error submitting compute operations!");
		}
	}

	void VulkanRenderTarget::SubmitGraphicsQueue(const FrameData& frameData, bool present)
	{
		// Graphics Command Buffer
		VkCommandBufferSubmitInfo cmdBufferSubmitInfo{};
		cmdBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
		cmdBufferSubmitInfo.pNext = nullptr;
		cmdBufferSubmitInfo.commandBuffer = frameData.graphicsCommandBuffer;
		cmdBufferSubmitInfo.deviceMask = 0;

		// Prepare for submission
		VkSubmitInfo2 submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
		submitInfo.pNext = nullptr;

		// Attach command buffer
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &cmdBufferSubmitInfo;

		// The compute semaphore still needs to be submitted here
		VkSemaphoreSubmitInfo computeSemaphoreInfo{};
		computeSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		computeSemaphoreInfo.pNext = nullptr;
		computeSemaphoreInfo.semaphore = frameData.computeSemaphore;
		computeSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
		computeSemaphoreInfo.deviceIndex = 0;
		computeSemaphoreInfo.value = 1;

		// Render Finished Semaphore
		VkSemaphoreSubmitInfo renderSemaphoreSubmitInfo{};
		renderSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		renderSemaphoreSubmitInfo.pNext = nullptr;
		renderSemaphoreSubmitInfo.semaphore = frameData.renderSemaphore;
		renderSemaphoreSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
		renderSemaphoreSubmitInfo.deviceIndex = 0;
		renderSemaphoreSubmitInfo.value = 1;

		// Attach signal semaphore (render)
		submitInfo.signalSemaphoreInfoCount = 1;
		submitInfo.pSignalSemaphoreInfos = &renderSemaphoreSubmitInfo;

		if (!present)
		{
			// Attach compute wait semaphore
			submitInfo.waitSemaphoreInfoCount = 1;
			submitInfo.pWaitSemaphoreInfos = &computeSemaphoreInfo;

			// Submit graphics operations and return early
			if (vkQueueSubmit2(frameData.graphicsQueue, 1, &submitInfo, frameData.renderFence) != VK_SUCCESS)
			{
				INDY_CORE_ERROR("Error Submitting Graphics Queue!");
				return;
			}
		}

		// Image available semaphore
		VkSemaphoreSubmitInfo swapchainSemaphoreSubmitInfo{};
		swapchainSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		swapchainSemaphoreSubmitInfo.pNext = nullptr;
		swapchainSemaphoreSubmitInfo.semaphore = frameData.imageAvailableSemaphore;
		swapchainSemaphoreSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR;
		swapchainSemaphoreSubmitInfo.deviceIndex = 0;
		swapchainSemaphoreSubmitInfo.value = 1;

		// Attach wait semaphore (image available)
		VkSemaphoreSubmitInfo waitSemaphores[] = { computeSemaphoreInfo, swapchainSemaphoreSubmitInfo };
		submitInfo.waitSemaphoreInfoCount = 2;
		submitInfo.pWaitSemaphoreInfos = waitSemaphores;

		// Queue up graphics operations
		if (vkQueueSubmit2(frameData.graphicsQueue, 1, &submitInfo, frameData.renderFence) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Error Submitting Graphics Queue!");
		}
	}

	void VulkanRenderTarget::SubmitImmediateCommand(std::function<void(const VkCommandBuffer&)>&& function)
	{
		// Reset compute fence
		if (vkResetFences(m_DeviceHandle->Get(), 1, &m_ImmediateSyncObjects->GetFence("Immediate Submit")) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Error resetting Render Fence");
			return;
		}

		m_ImmediateCommandPool->BeginCommandBuffer(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		function(m_ImmediateCommandPool->GetCommandBuffer(0));

		m_ImmediateCommandPool->EndCommandBuffer(0);

		VkSubmitInfo2 submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
		submitInfo.pNext = nullptr;

		VkCommandBufferSubmitInfo bufferSubmitInfo{};
		bufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
		bufferSubmitInfo.commandBuffer = m_ImmediateCommandPool->GetCommandBuffer(0);
		bufferSubmitInfo.pNext = nullptr;
		bufferSubmitInfo.deviceMask = 0;

		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &bufferSubmitInfo;

		// Submit graphics operations and return early
		if (vkQueueSubmit2(m_DeviceHandle->Queues()->GetGraphicsQueue(), 1, &submitInfo, m_ImmediateSyncObjects->GetFence("Immediate Submit")) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Error on Immediate Submit to Graphics Queue!");
		}

		if (vkWaitForFences(m_DeviceHandle->Get(), 1, &m_ImmediateSyncObjects->GetFence("Immediate Submit"), VK_TRUE, UINT64_MAX) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("Error waiting for Immediate Submit to graphics queue!");
		}
	}

	FrameData VulkanRenderTarget::GetCurrentFrameData()
	{
		auto frame = m_Frames[m_FrameCount % g_Max_Frames_In_Flight].get();

		FrameData data{};
		data.swapchainImageIndex = 0;
		data.computeQueue = m_DeviceHandle->Queues()->GetComputeQueue();
		data.graphicsQueue = m_DeviceHandle->Queues()->GetGraphicsQueue();
		data.computeFence = frame->GetFence("Compute");
		data.computeSemaphore = frame->GetSemaphore("Compute");
		data.renderFence = frame->GetFence("Render");
		data.renderSemaphore = frame->GetSemaphore("Render");
		data.imageAvailableSemaphore = frame->GetSemaphore("Swapchain");
		data.computeCommandPool = frame->GetComputeCommandPool();
		data.graphicsCommandPool = frame->GetGraphicsCommandPool();
		data.computeCommandBuffer = frame->GetComputeCommandBuffer(0);
		data.graphicsCommandBuffer = frame->GetGraphicsCommandBuffer(0);

		return data;
	}

	// TEMP: MOVE LATER!!!!
	// --------------------------------------------------------------------------

	void VulkanRenderTarget::init_imgui(const VkInstance& instance, Window* window)
	{
		std::vector<VkDescriptorPoolSize> pool_sizes = 
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER,					1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,			1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,			1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,	1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,	1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,			1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,			1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,	1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,	1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,		1000 }
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
		pool_info.pPoolSizes = pool_sizes.data();

		if (vkCreateDescriptorPool(m_DeviceHandle->Get(), &pool_info, nullptr, &m_ImGuiPool) != VK_SUCCESS)
		{
			INDY_CORE_ERROR("ImGui Initialization Error: Failed to create descriptor pool");
			return;
		}

		// Initialize core ImGui structures
		ImGui::CreateContext(/* shared font atlas */);

		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontDefault();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		// Initialize ImGui for GLFW
		if (!ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow*>(window->NativeWindow()), true))
		{
			INDY_CORE_ERROR("ImGui Initialization Error: Failed to initialize ImGui for GLFW");
			return;
		}

		// this initializes imgui for Vulkan
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = instance;
		init_info.PhysicalDevice = m_DeviceHandle->GetPhysicalDevice()->handle;
		init_info.Device = m_DeviceHandle->Get();
		init_info.QueueFamily = m_DeviceHandle->GetPhysicalDevice()->queueFamilies.graphics.value();
		init_info.Queue = m_DeviceHandle->Queues()->GetGraphicsQueue();
		init_info.DescriptorPool = m_ImGuiPool;
		init_info.MinImageCount = 3;
		init_info.ImageCount = 3;
		init_info.UseDynamicRendering = true;

		// Rendering Pipeline info
		VkPipelineRenderingCreateInfoKHR pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		pipelineInfo.colorAttachmentCount = 1;
		pipelineInfo.pColorAttachmentFormats = &m_Swapchain->GetFormat().format;
		pipelineInfo.pNext = nullptr;

		// Dynamic rendering parameters
		init_info.PipelineRenderingCreateInfo = pipelineInfo;

		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		if (!ImGui_ImplVulkan_Init(&init_info))
		{
			INDY_CORE_ERROR("Error Initializing Vulkan ImGui Impl");
		}

		if (!ImGui_ImplVulkan_CreateFontsTexture())
		{
			INDY_CORE_ERROR("Error Initializing Vulkan ImGui font texture");
		}
	}

	void VulkanRenderTarget::draw_imgui(const VkCommandBuffer& buffer, const VkImageView& imageView)
	{
		VkRenderingAttachmentInfo colorAttachment{};
		colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		colorAttachment.pNext = nullptr;

		colorAttachment.imageView = imageView;
		colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		VkRect2D renderArea{};
		renderArea.extent = m_Swapchain->GetExtent();

		VkRenderingInfo renderInfo{};
		renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderInfo.colorAttachmentCount = 1;
		renderInfo.pColorAttachments = &colorAttachment;
		renderInfo.renderArea = renderArea;
		renderInfo.flags = 0;
		renderInfo.layerCount = 1;

		vkCmdBeginRendering(buffer, &renderInfo);

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), buffer);

		vkCmdEndRendering(buffer);

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}
}
