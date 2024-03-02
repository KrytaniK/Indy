#include "Engine/Core/LogMacros.h"

#include <memory>
#include <typeindex>
#include <map>

import Indy_Core_EventSystem;

namespace Indy
{
	std::map<std::type_index, ListenerList> EventManager::s_EventListeners;
	std::map<std::type_index, std::vector<size_t>> EventManager::s_EmptyIndices;

	void IEventListener::Exec(IEvent* event)
	{
		this->Internal_Exec(event);
	}

	void EventManager::RemoveEventListener(const IEventHandle& handle)
	{
		// Find the listener list for the event type we're searching for
		auto it = EventManager::s_EventListeners.find(handle.eventID);

		// If it's not found, the an event listener was never registered with that type.
		if (it == EventManager::s_EventListeners.end())
		{
			INDY_CORE_ERROR("Could not remove event listener: Event not registered!");
			return;
		}

		// Remove the listener from the listener list
		it->second.erase(it->second.begin() + handle.index);
	}
}