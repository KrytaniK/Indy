#pragma once

#include "../RenderContext.h"
#include "Util.h"

#include <vector>
#include <memory>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace Engine::VulkanAPI
{
	class Context : public RenderContext
	{
	private:
		static VkInstance s_Instance;

		// These are essentially the render contexts (windows/viewports)
		static std::vector<Viewport> s_Viewports;

	public:
		Context();
		~Context();

	public:
		virtual void Init(void* window) override;
		virtual void Shutdown() override;
		virtual void CreateContext(void* window) override;

		virtual void BeginFrame() override;
		virtual void EndFrame(Camera& camera) override;
		virtual void DrawFrame() override;
		virtual void SwapBuffers() override;

		virtual void Submit(void* vertices, uint32_t vertexCount, void* indices, uint32_t indexCount, uint32_t instanceCount) override;

		//virtual void onWindowResize(Event& event) override;

	private:
		static std::vector<const char*> GetRequiredExtensions();

	private:
		bool m_FramebufferResized = false;
	};
}