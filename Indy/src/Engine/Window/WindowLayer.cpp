#include "Engine/Core/LogMacros.h"

#include <GLFW/glfw3.h>

import Indy_Core_Events;
import Indy_Core_WindowLayer;

namespace Indy
{
	static void GLFWErrorCallback(int error, const char* description)
	{
		INDY_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	void WindowLayer::onAttach()
	{
		// Initialize GLFW
		if (glfwInit() == GLFW_FALSE)
		{
			INDY_CORE_CRITICAL("[WindowLayer] Could not initialize GLFW!");
			return;
		}

		// Set GLFW Error Callback
		glfwSetErrorCallback(GLFWErrorCallback);

		// Initialize window manager
		m_WindowManager = std::make_shared<WindowManager>();

		// Bind WindowLayerActions
		m_LayerActions.resize(3);
		m_LayerActions[0] = std::make_unique<ICL_WindowAction_Request>(m_WindowManager.get());
		m_LayerActions[1] = std::make_unique<ICL_WindowAction_Create>(m_WindowManager.get());
		m_LayerActions[2] = std::make_unique<ICL_WindowAction_Destroy>(m_WindowManager.get());

		// Bind Event Listener
		m_EventHandles.emplace_back(
			EventManagerCSR::AddEventListener<WindowLayer, ILayerEvent>(this, &WindowLayer::onEvent)
		);
	}

	void WindowLayer::onDetach() 
	{

	};

	void WindowLayer::Update()
	{
		m_WindowManager->Update();
	}

	void WindowLayer::onEvent(ILayerEvent* event)
	{
		if (event->targetLayer != "ICL_Window")
			return;

		uint8_t action = static_cast<uint8_t>(((ICL_WindowEvent*)event)->action);
		m_LayerActions[action]->Execute(event->layerData);
	}
}
