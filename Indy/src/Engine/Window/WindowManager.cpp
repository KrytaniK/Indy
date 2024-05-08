#include "Engine/Core/LogMacros.h"

#include <memory>
#include <string>

import Indy_Core_Window;

namespace Indy
{
	WindowManager::WindowManager()
	{
		// Pre-allocate 20 windows and window handles. Will help avoid reallocations.
		m_Windows.reserve(20);
		m_Handles.resize(20);
	}

	WindowManager::~WindowManager()
	{

	}

	void WindowManager::Update()
	{
		for (const auto& window : m_Windows)
		{
			if (window && !window->Properties().minimized)
				window->Update();
		}
	}

	IWindowHandle& WindowManager::GetWindow(uint8_t index)
	{
		if (index >= m_Handles.size())
		{
			INDY_CORE_ERROR(
				"Could not get window at index [{0}]. Index out of bounds. Returning handle to first window", 
				index
			);
			return m_Handles[0];
		}

		return m_Handles[index];
	}

	IWindowHandle& Indy::WindowManager::GetActiveWindow()
	{
		uint8_t index = 0;
		for (const auto& window : m_Windows)
		{
			if (window->Properties().focused)
				return m_Handles[index]; // Return the first focused window

			index++;
		}

		// If there's no window in focus, return the first window
		INDY_CORE_WARN("No Window in focus...");
		return m_Handles[0];
	}

	IWindowHandle& WindowManager::AddWindow(WindowCreateInfo& createInfo)
	{
		// Ensure We're not attempting to recreate an existing window
		uint8_t index = 0;
		for (const auto& window : m_Windows)
		{
			if (window->Properties().title == createInfo.title ||
				window->Properties().id == createInfo.id)
			{
				INDY_CORE_WARN(
					"Failed to create window [{0}]. Window already exists!",
					createInfo.title
				);
				return m_Handles[index]; // If a matching window was found, return its handle
			}
			index++;
		}

		IWindowHandle handle;

		// Find out where the index will be
		if (m_EmptyWindows.empty())
		{
			handle.index = (uint8_t)m_Windows.size();
		}
		else
		{
			handle.index = m_EmptyWindows.front();
			m_EmptyWindows.pop();
		}

		// We need to attach the index to the createInfo.
		createInfo.id = handle.index;

		// Create Platform-Specific Window
		std::shared_ptr<IWindow> window;
	#ifdef ENGINE_PLATFORM_WINDOWS
		window = std::make_shared<WindowsWindow>(createInfo);
	#else
		INDY_CORE_ERROR("Could not create window: Unsupported Platform!");
	#endif // ENGINE_PLATFORM_WINDOWS

		if (handle.index == (uint8_t)m_Windows.size())
			m_Windows.emplace_back(window);
		else
		{
			if (m_Windows[handle.index])
			{
				INDY_CORE_ERROR("Failed to insert window at index [{0}]. Window exists!", handle.index);
				return m_Handles[handle.index];
			}

			m_Windows[handle.index] = window;
		}

		// Store a weak pointer to the window on the handle and increment the window count
		handle.window = window;
		m_WindowCount++;

		// Copy the handle and store on window manager
		if (handle.index == m_Handles.size())
			m_Handles.emplace_back(handle); // Force reallocation if we have more windows than the preallocated amount
		else
			m_Handles[handle.index] = handle;

		// Return the copied window handle.
		return m_Handles[handle.index];
	}

	void WindowManager::DestroyWindow(uint8_t index)
	{
		if (index >= m_Windows.size())
		{
			INDY_CORE_ERROR(
				"Could not destory window [{0}]. Index out of array bounds.", 
				index
			);
			return;
		}

		m_EmptyWindows.push(index);
		m_Windows[index] = nullptr;
		m_WindowCount--;
	}

}