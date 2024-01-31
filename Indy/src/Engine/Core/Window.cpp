#include "Engine/Platform/Windows/WindowsWindow.h"

#include "Engine/Core/LogMacros.h"

import Indy_Core;
import EventSystem;

namespace Indy
{
	void OpenWindow(const WindowCreateInfo& createInfo)
	{
		WindowCreateEvent event{};
		event.createInfo = createInfo;
		EventManager::Notify<WindowCreateEvent>(&event);
	}
}

namespace Engine
{
	std::unique_ptr<Window> Window::Create(const WindowSpec& spec)
	{
		#ifdef ENGINE_PLATFORM_WINDOWS
		return std::make_unique<WindowsWindow>(spec);
		#else
		INDY_CORE_CRITICAL("Window Could Not Be Created: 'Platform Not Supported'");
		return nullptr;
		#endif
	}
}