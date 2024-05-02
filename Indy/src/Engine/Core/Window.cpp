import Indy_Core;
import Indy_Core_Window;
import Indy_Core_Events;

namespace Indy
{
	void OpenWindow(const WindowCreateInfo& createInfo)
	{
		WindowCreateEvent event{};
		event.createInfo = createInfo;
		EventManager::Notify<WindowCreateEvent>(event);
	}
}