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
		m_EventHandles.emplace_back(
			EventManagerCSR::AddEventListener<WindowLayer, ILayerEvent>(this, &WindowLayer::onEvent)
		);

		if (glfwInit() == GLFW_FALSE)
		{
			INDY_CORE_CRITICAL("[WindowLayer] Could not initialize GLFW!");
			return;
		}

		glfwSetErrorCallback(GLFWErrorCallback);
	}

	void WindowLayer::onDetach() 
	{

	};

	void WindowLayer::Update()
	{
		// Delete any Windows Queued for deletion
		while (!m_WindowDeleteQueue.empty())
		{
			uint8_t winID = m_WindowDeleteQueue.front();
			m_WindowDeleteQueue.pop();

			DestroyWindow(winID);
		}

		// Update any remaining windows
		for (uint8_t i = 0; i < m_Windows.size(); i++)
			m_Windows.at(i)->Update();
	}

	void WindowLayer::onEvent(ILayerEvent* event)
	{
		if (event->targetLayer != "ICL_Window")
			return;

		// Automatically stop event propagation
		event->propagates = false;

		
	}

	void WindowLayer::OnWindowCreate(WindowCreateEvent* event)
	{
		// Stop event propagation
		event->propagates = false;

		// Generate Unique Window ID
		event->createInfo.id = GenerateWindowID();

		// Increment Window Count
		m_WindowCount++;

		// Store vector index for this window
		m_WindowIndices.emplace(event->createInfo.id, static_cast<uint8_t>(m_Windows.size()));

		// Generate Window
		#ifdef ENGINE_PLATFORM_WINDOWS
			m_Windows.emplace_back(std::make_unique<WindowsWindow>(event->createInfo));
		#else
			INDY_CORE_ERROR("Could not create window: Unsupported Platform!");
		#endif // ENGINE_PLATFORM_WINDOWS
	}

	void WindowLayer::OnWindowDestroy(WindowDestroyEvent* event)
	{
		// Stop event propagation
		event->propagates = false;

		m_WindowDeleteQueue.push(event->id);

		// decrement window count
		if (--m_WindowCount > 0)
			return;

		// Trigger App Termination if no windows remain
		/*ApplicationEvent terminateEvent;
		terminateEvent.terminate = true;
		EventManager::Notify<ApplicationEvent>(terminateEvent);*/
	}

	void WindowLayer::OnRequestWindow(WindowRequestEvent* event)
	{
		// Stop event propagation
		event->propagates = false;

		// Always return the window if there is only one window
		if (m_Windows.size() == 1)
		{
			event->window = m_Windows.at(0).get();
			return;
		}

		for (uint8_t i = 0; i < m_Windows.size(); i++)
		{
			// Attach the first 'focused' window
			if (m_Windows.at(i)->Properties().focused)
				event->window = m_Windows.at(i).get();
		}
	}

	uint8_t WindowLayer::GenerateWindowID()
	{
		uint8_t windowID = rand() % (UINT8_MAX * 2 + 1); // Unsigned value between 0 and 255

		// Ensure a window with that ID does not exist
		if (m_WindowIndices.find(windowID) != m_WindowIndices.end())
			windowID = GenerateWindowID();
		
		return windowID;
	}

	void WindowLayer::DestroyWindow(uint8_t id)
	{
		// Find the target vector index
		const auto indexIt = m_WindowIndices.find(id);
		if (indexIt == m_WindowIndices.end())
		{
			INDY_CORE_ERROR("Index for Window with ID {0} not found!", id);
			return;
		}

		// Decrement the indices for windows that occured AFTER this one.
		for (auto& pair : m_WindowIndices)
		{
			if (pair.second > indexIt->second)
				pair.second--;
		}

		// Remove Window from vector
		m_Windows.erase(m_Windows.begin() + indexIt->second);
	}
}
