#include "LogMacros.h"
#include "ProfileMacros.h"

#include <memory>
#include <typeindex>
#include <map>

import EventSystem;

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
		auto it = EventManager::s_EventListeners.find(handle.eventID);

		if (it == EventManager::s_EventListeners.end())
		{
			INDY_CORE_ERROR("Could not remove event listener: Event not registered!");
			return;
		}

		it->second.at(handle.index) = nullptr;

		auto indexIt = EventManager::s_EmptyIndices.find(handle.eventID);
		if (indexIt == EventManager::s_EmptyIndices.end())
		{
			EventManager::s_EmptyIndices.emplace(handle.eventID, std::vector<size_t>(handle.index));
		}
	}
}