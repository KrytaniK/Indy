#include "Engine/Core/LogMacros.h"

#include <memory>
#include <typeindex>
#include <map>

import Indy_Core_Events;

namespace Indy
{
	std::vector<std::pair<std::type_index, uint32_t>> EventManagerCSR::s_TypeIndices;
	std::vector<std::shared_ptr<IEventListener>> EventManagerCSR::s_EventListeners;

	std::unordered_map<std::type_index, ListenerList> EventManager::s_EventListeners;
	std::unordered_map<std::type_index, std::vector<size_t>> EventManager::s_EmptyIndices;

	void IEventListener::Exec(IEvent& event)
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
		it->second.at(handle.index) = nullptr;
	}
}