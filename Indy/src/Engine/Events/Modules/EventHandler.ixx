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
			EventHandler();
			~EventHandler() = default;

			void Notify(IEvent* event);
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

		private:
			uint32_t m_DelegateCount;
			std::vector<std::shared_ptr<EventDelegate>> m_Listeners;
		};

		// Since Implementation units aren't exported, template definitions must be places in the Module Interface Unit

		template<typename EventType, class C>
		std::shared_ptr<EventDelegate> EventHandler::Subscribe(C* instance, void(C::* callback)(EventType*))
		{
			std::shared_ptr<EventDelegate> shared_delegate = std::make_shared<EventDelegate>(m_DelegateCount++);
			shared_delegate->Bind<EventType, C>(instance, callback);

			m_Listeners.emplace_back(shared_delegate);
			return shared_delegate;
		}

		template<class C>
		std::shared_ptr<EventDelegate> EventHandler::Subscribe(C* instance, void(C::* callback)())
		{
			std::shared_ptr<EventDelegate> shared_delegate = std::make_shared<EventDelegate>(m_DelegateCount++);
			shared_delegate->Bind<C>(instance, callback);

			m_Listeners.emplace_back(shared_delegate);
			return shared_delegate;
		}

		template<typename EventType>
		std::shared_ptr<EventDelegate> EventHandler::Subscribe(const std::function<void(EventType*)>& callback)
		{
			static_assert(std::is_base_of_v<IEvent, EventType>, "[Events] Template parameter <EventType> must derive from base <IEvent>.");

			std::shared_ptr<EventDelegate> shared_delegate = std::make_shared<EventDelegate>(m_DelegateCount++);
			shared_delegate->Bind<EventType>(callback);

			m_Listeners.emplace_back(shared_delegate);
			return shared_delegate;
		}
	}
}