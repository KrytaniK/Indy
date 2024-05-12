module;

#include <functional>
#include <type_traits>
#include <memory>

export module Indy.Events;

export import :Handler;
export import :Delegate;
export import :Types;

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
				s_EventHandler.Notify(event);
			};

			inline static void Subscribe(EventDelegate& eventDelegate)
			{
				s_EventHandler.Subscribe(eventDelegate);
			};

			inline static std::shared_ptr<EventDelegate> Subscribe(const std::function<void(EventType*)>& func)
			{
				return s_EventHandler.Subscribe(func);
			}

			template<class C>
			inline static std::shared_ptr<EventDelegate> Subscribe(C* instance, void(C::* callback)(EventType*))
			{
				return s_EventHandler.Subscribe<EventType, C>(instance, callback);
			}

			inline static void UnSubscribe(const EventDelegate& eventDelegate)
			{
				s_EventHandler.UnSubscribe(eventDelegate);
			};

		private:
			Events() = default;
			~Events() = default;

		private:
			inline static EventHandler s_EventHandler;
		};
	}
}