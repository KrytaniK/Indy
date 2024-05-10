module;

#include <functional>

export module Indy_Core_Events;

export import Indy_Core_EventHandler;

export
{
	namespace Indy
	{
		template<typename EventType>
		class Events
		{
			static_assert(std::is_base_of<IEvent, EventType>::value, "[Events] Template parameter <EventType> must derive from base <IEvent>.");

		public:
			inline static void Notify(IEvent* event)
			{
				s_TypedHandler.Notify(event);
			};

			inline static void Subscribe(TEventDelegate<EventType>& eventDelegate)
			{
				s_TypedHandler.Subscribe(eventDelegate);
			};

			inline static TEventDelegate<EventType> Subscribe(const std::function<void(EventType*)>& func)
			{
				return s_TypedHandler.Subscribe(func);
			}

			template<class C>
			inline static TEventDelegate<EventType> Subscribe(C* instance, void(C::* callback)(EventType*))
			{
				return s_TypedHandler.Subscribe<C>(instance, callback);
			}

			inline static void UnSubscribe(const TEventDelegate<EventType>& eventDelegate)
			{
				s_TypedHandler.UnSubscribe(eventDelegate);
			};

		private:
			Events() = default;
			~Events() = default;

		private:
			inline static TEventHandler<EventType> s_TypedHandler;
		};
	}
}