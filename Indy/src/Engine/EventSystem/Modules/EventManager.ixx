module;

#include <vector>
#include <memory>
#include <functional>
#include <typeindex>
#include <map>

export module Indy_Core_EventSystem:EventManager;

import :EventHandle;
import :EventListener;

export namespace Indy
{
	typedef std::vector<std::shared_ptr<IEventListener>> ListenerList;

	class EventManager
	{
	private:
		EventManager();

	public:
		template<class EventType>
		static IEventHandle AddEventListener(std::function<void(EventType*)>);

		template<class C, typename EventType>
		static IEventHandle AddEventListener(C* instance, void(C::* callback)(EventType*));

		template<typename EventType>
		static void Notify(EventType* event);

		static void RemoveEventListener(const IEventHandle& handle);

	private:
		static std::map<std::type_index, ListenerList> s_EventListeners;
		static std::map<std::type_index, std::vector<size_t>> s_EmptyIndices;
	};

	template<class EventType>
	IEventHandle EventManager::AddEventListener(std::function<void(EventType*)> callback)
	{
		std::type_index id = std::type_index(typeid(EventType));

		// Generate the handle to be returned;
		IEventHandle handle{ id };

		// Find out which event we're targeting
		auto it = s_EventListeners.find(id);

		// If it's not found, this is the first event listener of this type
		if (it == s_EventListeners.end())
		{
			s_EventListeners.emplace(id, ListenerList{ std::make_shared<EventListener<EventType>>(callback) });
			handle.index = 0;
			return handle;
		}

		// Otherwise, push the listener info the listener list
		it->second.emplace_back(std::make_shared<EventListener<EventType>>(callback));
		handle.index = it->second.size() - 1;

		return handle;
	}

	template<class C, class EventType>
	IEventHandle EventManager::AddEventListener(C* instance, void(C::* callback)(EventType*))
	{
		std::type_index id = std::type_index(typeid(EventType));

		// Generate the handle to be returned;
		IEventHandle handle{ id };

		// Find out which event we're targeting
		auto it = s_EventListeners.find(id);

		// If it's not found, this is the first event listener of this type
		if (it == s_EventListeners.end())
		{
			s_EventListeners.emplace(id, ListenerList{ std::make_shared<MemberEventListener<C, EventType>>(instance, callback) });
			handle.index = 0;
			return handle;
		}

		// Otherwise, push the listener info the listener list
		it->second.emplace_back(std::make_shared<MemberEventListener<C, EventType>>(instance, callback));
		handle.index = it->second.size() - 1;

		return handle;
	}

	template<typename EventType>
	void EventManager::Notify(EventType* event)
	{
		using namespace std;

		// Find the entry for this event type in the event listeners list
		auto it = s_EventListeners.find(std::type_index(typeid(EventType)));

		// Bail if it doesn't exist
		if (it == s_EventListeners.end())
		{
			return;
		}

		if (event->bubbles)
		{
			for (auto listIt = it->second.rbegin(); listIt != it->second.rend(); ++listIt)
				listIt->get()->Exec(event);
		}
		else
		{
			for (auto listIt = it->second.begin(); listIt != it->second.end(); ++listIt)
				listIt->get()->Exec(event);
		}
	}
}