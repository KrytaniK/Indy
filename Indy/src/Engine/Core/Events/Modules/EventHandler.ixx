module;

#include <cstdint>
#include <vector>
#include <functional>
#include <memory>

export module Indy.Events:Handler;

import :Delegate;
import :Types;

export
{
	namespace Indy
	{
		class EventHandler
		{
		public:
			EventHandler() = default;
			~EventHandler() = default;

			void Notify(IEvent* event);
			void Notify(bool bubbles);
			void Notify();

			template<typename EventType, class C>
			std::shared_ptr<EventDelegate> Subscribe(C* instance, void(C::* callback)(EventType*));

			template<class C>
			std::shared_ptr<EventDelegate> Subscribe(C* instance, void(C::* callback)());

			template<typename EventType>
			std::shared_ptr<EventDelegate> Subscribe(const std::function<void(EventType*)>& callback);
			std::shared_ptr<EventDelegate> Subscribe(const std::function<void()>& callback);

			std::shared_ptr<EventDelegate> Subscribe(EventDelegate& eventDelegate);

			void Subscribe(std::shared_ptr<EventDelegate>& eventDelegate);

			void UnSubscribe(const EventDelegate& eventDelegate);

			template<typename EventType>
			void operator()(EventType* event);
			void operator()();
			void operator()(bool bubbles);
			void operator+=(EventDelegate& eventDelegate);
			void operator+=(const std::function<void()>& callback);
			void operator-=(const EventDelegate& eventDelegate);

		private:
			uint32_t delegateCount = 0;
			std::vector<std::shared_ptr<EventDelegate>> m_Listeners;
		};

		// Since Implementation units aren't exported, template definitions must be places in the Module Interface Unit

		template<typename EventType, class C>
		std::shared_ptr<EventDelegate> EventHandler::Subscribe(C* instance, void(C::* callback)(EventType*))
		{
			std::shared_ptr<EventDelegate> sharedDelegate = std::make_shared<EventDelegate>(delegateCount++);
			sharedDelegate->Bind<EventType, C>(instance, callback);

			m_Listeners.emplace_back(sharedDelegate);
			return sharedDelegate;
		}

		template<class C>
		std::shared_ptr<EventDelegate> EventHandler::Subscribe(C* instance, void(C::* callback)())
		{
			std::shared_ptr<EventDelegate> sharedDelegate = std::make_shared<EventDelegate>(delegateCount++);
			sharedDelegate->Bind<C>(instance, callback);

			m_Listeners.emplace_back(sharedDelegate);
			return sharedDelegate;
		}

		template<typename EventType>
		std::shared_ptr<EventDelegate> EventHandler::Subscribe(const std::function<void(EventType*)>& callback)
		{
			static_assert(std::is_base_of<IEvent, EventType>::value, "[Events] Template parameter <EventType> must derive from base <IEvent>.");

			std::shared_ptr<EventDelegate> sharedDelegate = std::make_shared<EventDelegate>(delegateCount++);
			sharedDelegate->Bind<EventType>(callback);

			m_Listeners.emplace_back(sharedDelegate);
			return sharedDelegate;
		}

		template<typename EventType>
		void EventHandler::operator()(EventType* event)
		{
			static_assert(std::is_base_of<IEvent, EventType>::value, "[Events] Template parameter <EventType> must derive from base <IEvent>.");

			Notify(event);
		}
	}
}