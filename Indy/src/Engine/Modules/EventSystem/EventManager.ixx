module;

#include <vector>
#include <memory>
#include <functional>
#include <typeindex>
#include <map>

#include "Engine/Core/LogMacros.h"

export module EventSystem:EventManager;

export
{
	namespace Indy
	{
		class IEventListener;
		struct IEvent;

		template<class EventType>
		class EventListener;

		template<class C, class EventType>
		class MemberEventListener;

		struct IEventHandle;

		typedef std::vector<std::shared_ptr<IEventListener>> ListenerList;

		class EventManager
		{
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
			IEventHandle handle{ id };

			auto it = s_EventListeners.find(id);

			if (it == s_EventListeners.end())
			{
				// First time adding an event listener of this type
				ListenerList listeners;
				listeners.emplace_back(std::make_shared<EventListener<EventType>>(callback));
				s_EventListeners.emplace(id, listeners);
				handle.index = 0;
				return handle;
			}

			// Check for empy indices
			auto indexIt = s_EmptyIndices.find(id);

			if (indexIt == s_EmptyIndices.end())
			{
				// No empty indices
				handle.index = it->second.size();
				it->second.emplace_back(std::make_shared<EventListener<EventType>>(callback));
				return handle;
			}

			// Grab the most recently emptied index and insert there
			handle.index = indexIt->second.back();
			indexIt->second.pop_back();
			it->second.insert(it->second.begin() + handle.index, std::make_shared<EventListener<EventType>>(callback));

			return handle;
		}

		template<class C, class EventType>
		IEventHandle EventManager::AddEventListener(C* instance, void(C::* callback)(EventType*))
		{
			std::type_index id = std::type_index(typeid(EventType));
			IEventHandle handle{ id };

			auto it = s_EventListeners.find(id);

			if (it == s_EventListeners.end())
			{
				// First time adding an event listener of this type
				ListenerList listeners;
				listeners.emplace_back(std::make_shared<MemberEventListener<C, EventType>>(instance, callback));
				s_EventListeners.emplace(id, listeners);
				handle.index = 0;
				return handle;
			}

			// Check for empy indices
			auto indexIt = s_EmptyIndices.find(id);

			if (indexIt == s_EmptyIndices.end())
			{
				// No empty indices
				handle.index = it->second.size();
				it->second.emplace_back(std::make_shared<MemberEventListener<C, EventType>>(instance, callback));
				return handle;
			}

			// Grab the most recently emptied index and insert there
			handle.index = indexIt->second.back();
			indexIt->second.pop_back();
			it->second.insert(it->second.begin() + handle.index, std::make_shared<MemberEventListener<C, EventType>>(instance, callback));

			return handle;
		}

		template<typename EventType>
		void EventManager::Notify(EventType* event)
		{
			auto it = s_EventListeners.find(std::type_index(typeid(EventType)));

			if (it == s_EventListeners.end())
			{
				INDY_CORE_ERROR("No Listeners for event type");
				return;
			}

			for (size_t i = 0; i < it->second.size(); i++)
			{
				if (it->second.at(i) == nullptr)
					continue;

				it->second.at(i)->Exec(event);
			}
		}
	}
}