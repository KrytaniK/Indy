#include <Engine/Core/LogMacros.h>

#include <memory>
#include <string>

import Indy.Window;
import Indy.WindowsWindow;

namespace Indy
{
	WindowManager::WindowManager()
	{
		// Pre-allocate 20 windows and window handles. Will help avoid reallocations.
		m_Windows.reserve(20);
	}

	WindowManager::~WindowManager()
	{

	}

	void WindowManager::Update()
	{
		for (uint8_t i = 0; i < m_WindowCount; i++)
		{
			// If there is no window, or it's minimized
			if (!m_Windows[i] || m_Windows[i]->Properties().minimized)
				continue;

			// If the window has been destroyed, remove it from the window vector
			if (!m_Windows[i]->NativeWindow())
			{
				DestroyWindow(i);
				continue;
			}

			// Otherwise, update the window
			m_Windows[i]->Update();
		}
	}

	const IWindow* WindowManager::GetWindow(uint8_t index) const
	{
		if (index >= m_WindowCount)
		{
			INDY_CORE_ERROR(
				"Could not get window at index [{0}]. Index out of bounds. Returning handle to first window", 
				index
			);
			return nullptr;
		}

		return m_Windows[index].get();
	}

	const IWindow* WindowManager::GetActiveWindow() const
	{
		uint8_t index = 0;
		for (const auto& window : m_Windows)
		{
			if (window->Properties().focused)
				return window.get(); // Return the first focused window

			index++;
		}

		// If there's no window in focus, return the first window
		INDY_CORE_WARN("No Window in focus...");
		return nullptr;
	}

	void WindowManager::AddWindow(WindowCreateInfo& createInfo)
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
				return; // If a matching window was found, return its handle
			}
			index++;
		}

		// Create Platform-Specific Window
		std::shared_ptr<IWindow> window;
	#ifdef ENGINE_PLATFORM_WINDOWS
		window = std::make_shared<WindowsWindow>(createInfo);
	#else
		INDY_CORE_ERROR("Could not create window: Unsupported Platform!");
	#endif // ENGINE_PLATFORM_WINDOWS

		// Store the window on the handle and increment the window count
		m_Windows.emplace_back(window);
		m_WindowCount++;
	}

	void WindowManager::DestroyWindow(uint8_t index)
	{
		if (index >= m_WindowCount)
		{
			INDY_CORE_ERROR(
				"Could not destory window [{0}]. Index out of array bounds.", 
				index
			);
			return;
		}

		// If we're not referencing the last window
		if (index != m_Windows.size() - 1)
		{
			// Swap with last window
			std::shared_ptr<IWindow> toDelete = m_Windows[index];
			m_Windows[index] = m_Windows.back();
			m_Windows[m_Windows.size() - 1] = toDelete;
		}

		// Remove last element
		m_Windows.pop_back();
		m_WindowCount--;

		// Note: This only works because the order in which windows update do not matter.
	}

}