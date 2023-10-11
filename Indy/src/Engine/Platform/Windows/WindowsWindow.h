#pragma once

#include "Engine/Core/Window.h"
#include "Engine/EventSystem/Events.h"
#include "WindowsEvents.h"

// Vulkan (Temp)
#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace Engine
{
	class WindowsWindow : public Window
	{
		public:
			WindowsWindow(const WindowSpec& spec);
			virtual ~WindowsWindow();

			void onUpdate() override;

			unsigned int GetWidth() const override { return m_WindowSpec.width; };
			unsigned int GetHeight() const override { return m_WindowSpec.height; };

		private:

			void InitVulkan(); // This will be moved to a Renderer class/API in the future.
			VkInstance m_VulkanInstance;

			GLFWwindow* m_GLFW_Window;

			WindowSpec m_WindowSpec;
	};
}