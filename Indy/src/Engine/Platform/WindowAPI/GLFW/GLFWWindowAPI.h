#pragma once

#include "../WindowAPI.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	class GLFW_WindowAPI : public WindowAPI
	{
	public:
		GLFW_WindowAPI(const WindowSpec& windowSpec);
		virtual ~GLFW_WindowAPI();

		virtual void onUpdate() override;
		virtual void* GetWindow() override { return m_Window; };

	private:
		GLFWwindow* m_Window;
	};
}