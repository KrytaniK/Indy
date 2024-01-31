module;

#include <functional>

export module EventSystem:EventListener;

export
{
	namespace Indy
	{
		struct IEvent;

		class IEventListener
		{
		public:
			void Exec(IEvent* event);

		private:
			virtual void Internal_Exec(IEvent* event) = 0;
		};

		template<typename EventType>
		class EventListener : public IEventListener
		{
		public:
			EventListener(std::function<void(EventType*)> callback);

		private:
			virtual void Internal_Exec(IEvent* event) override;

		private:
			std::function<void(EventType*)> m_Callback;
		};

		template<class C, class EventType>
		class MemberEventListener : public IEventListener
		{
		public:
			typedef void(C::* MemberFunction)(EventType*);

			MemberEventListener(C* instance, MemberFunction callback);

		private:
			virtual void Internal_Exec(IEvent* event) override;

		private:
			C* m_Instance;
			MemberFunction m_Callback;
		};

		template<typename EventType>
		EventListener<EventType>::EventListener(std::function<void(EventType*)> callback)
			: m_Callback(callback) {}

		template<typename EventType>
		void EventListener<EventType>::Internal_Exec(IEvent* event)
		{
			m_Callback(static_cast<EventType*>(event));
		}

		template<class C, class EventType>
		MemberEventListener<C, EventType>::MemberEventListener(C* instance, MemberFunction callback)
			: m_Instance(instance), m_Callback(callback) {}

		template<class C, class EventType>
		void MemberEventListener<C, EventType>::Internal_Exec(IEvent* event)
		{
			(m_Instance->*m_Callback)(static_cast<EventType*>(event));
		}
	}
}