#include "Window.h"

#ifdef ENGINE_PLATFORM_WINDOWS
	#include "Engine/Platform/Windows/WindowsWindow.h"
#endif // ENGINE_PLATFORM_WINDOWS

namespace Engine
{
	std::unique_ptr<Window> Window::Create(const WindowSpec& spec)
	{
		#ifdef ENGINE_PLATFORM_WINDOWS
			return std::make_unique<WindowsWindow>(spec);
		return nullptr;
		#else
			INDY_CORE_CRITICAL("Window Could Not Be Created: 'Platform Not Supported'")
			return nullptr;
		#endif
	}
}
