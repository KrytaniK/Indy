module;

#include <Engine/Core/LogMacros.h>

#include <functional>
#include <cstdint>

export module Indy.Events:Delegate;

import :Types;

export
{
	namespace Indy
	{
		class EventDelegate
		{
		public:
			EventDelegate() = default;
			EventDelegate(const uint32_t& id);

			void Execute(IEvent* event);
			void Execute();

			template<typename EventType>
			void Bind(const std::function<void(EventType*)>& callback);
			void Bind(const std::function<void()>& callback);
			 
			template<typename EventType, class C>
			void Bind(C* instance, void(C::* callback)(EventType*));
			template<class C>
			void Bind(C* instance, void(C::* callback)());

		public:
			uint32_t m_ID;

		private:
			std::function<void()> m_BaseFunc;
			std::function<void(IEvent*)> m_EventFunc;
		};

		// Since Implementation units aren't exported, template definitions must be places in the Module Interface Unit

		template<typename EventType>
		void EventDelegate::Bind(const std::function<void(EventType*)>& callback)
		{
			static_assert(std::is_base_of_v<IEvent, EventType>, "[Events] Template parameter <EventType> must derive from base <IEvent>.");

			m_EventFunc = [callback](IEvent* event) { callback(static_cast<EventType*>(event)); };

		}

		template<class C>
		void EventDelegate::Bind(C* instance, void(C::* callback)())
		{
			m_BaseFunc = [instance, callback]() { (instance->*callback)(); };
		}

		template<typename EventType, class C>
		void EventDelegate::Bind(C* instance, void(C::* callback)(EventType*))
		{
			static_assert(std::is_base_of_v<IEvent, EventType>, "[Events] Template parameter <EventType> must derive from base <IEvent>.");

			m_EventFunc = [instance, callback](IEvent* event) { (instance->*callback)(static_cast<EventType*>(event)); }; 

		}
	}
}