module;

#include "Engine/Core/LogMacros.h"
#include <cstdint>
#include <vector>
#include <functional>

export module Indy_Core_EventHandler;

export import :EventDelegate;

/* TODO:
	- Implement:
		- IEvent
		- Event Bubbling
		- Event Propagation
*/

export
{
	namespace Indy
	{
		struct IEvent
		{
			bool bubbles = true;
			bool propagates = true;
		};

		class EventHandler
		{
		public:
			void Notify()
			{
				for (const EventDelegate& listener : m_Listeners)
					listener.Exec();
			};

			void Subscribe(EventDelegate& del)
			{
				// Always set the id.
				// Ensures all delegate IDs are 'unique'
				del.id = delegateCount++;

				*this += del;
			}

			EventDelegate Subscribe(const std::function<void()>& callback)
			{
				EventDelegate baseDelegate(delegateCount++);
				baseDelegate.Bind(callback);

				*this += baseDelegate;

				return baseDelegate;
			}

			template<class C>
			EventDelegate Subscribe(C* instance, void(C::* callback)())
			{
				EventDelegate baseDelegate(delegateCount++);
				baseDelegate.Bind<C>(instance, callback);

				*this += baseDelegate;

				return baseDelegate;
			}

			void UnSubscribe(const EventDelegate& del)
			{ *this -= del; }

			void operator+=(const EventDelegate& del)
			{
				for (const auto& listener : m_Listeners)
				{
					if (listener.id == del.id)
					{
						INDY_CORE_ERROR("Cannot subscribe with delegate ID [{0}]. Delegate with that ID already exists!");
						return;
					}
				}

				m_Listeners.emplace_back(del);
			};

			void operator-=(const EventDelegate& del)
			{
				if (m_Listeners.empty())
					return;

				// Find the delegate to remove
				size_t deleteIndex = SIZE_MAX;
				for (const auto& listener : m_Listeners)
				{
					++deleteIndex;
					if (listener.id == del.id)
						break;
				}

				// Bail if the delegate was not found
				if (deleteIndex == SIZE_MAX)
				{
					INDY_CORE_ERROR("Could not remove event delegate with id [{0}]. Delegate does not exist!", del.id);
					return;
				}

				// No need to swap if there is only one listener
				if (m_Listeners.size() == 1)
				{
					m_Listeners.pop_back();
					return;
				}

				// Swap the target element and the last element
				EventDelegate temp = m_Listeners[deleteIndex];
				m_Listeners[deleteIndex] = m_Listeners.back();
				m_Listeners[m_Listeners.size() - 1] = temp;

				// Remove last element (which should now be the target
				m_Listeners.pop_back();
			};

		private:
			uint32_t delegateCount = 0;
			std::vector<EventDelegate> m_Listeners;
		};

		template<typename EventType>
		class TEventHandler
		{
			static_assert(std::is_base_of<IEvent, EventType>::value, "[TEventHandler] Template parameter <EventType> must derive from base <IEvent>.");

		public:
			void Notify(IEvent* event)
			{
				EventType* tEvent = static_cast<EventType*>(event);

				// If event does not bubble, notify from start -> end
				if (!event->bubbles)
				{
					for (const TEventDelegate<EventType>& listener : m_Listeners)
					{
						if (!tEvent->propagates)
							return;

						listener.Exec(tEvent);
					}
				}
				else // Otherwise, notify from end -> start
				{
					for (auto iter = m_Listeners.rbegin(); iter != m_Listeners.rend(); ++iter)
					{
						if (!tEvent->propagates)
							return;

						(*iter).Exec(tEvent);
					}
				}
				
			};

			void Subscribe(TEventDelegate<EventType>& del)
			{ 
				// Always set the id.
				// Ensures all delegate IDs are 'unique'
				del.id = delegateCount++;

				*this += del;
			}

			TEventDelegate<EventType> Subscribe(const std::function<void(EventType*)>& callback)
			{
				TEventDelegate<EventType> typedDelegate(delegateCount++);
				typedDelegate.Bind(callback);

				*this += typedDelegate;

				return typedDelegate;
			}

			template<class C>
			TEventDelegate<EventType> Subscribe(C* instance, void(C::* callback)(EventType*))
			{
				TEventDelegate<EventType> typedDelegate(delegateCount++);
				typedDelegate.Bind<C>(instance, callback);

				*this += typedDelegate;

				return typedDelegate;
			}

			void UnSubscribe(const TEventDelegate<EventType>& del)
			{ *this -= del; }

			void operator+=(const TEventDelegate<EventType>& del)
			{
				for (const auto& listener : m_Listeners)
				{
					if (listener.id == del.id)
					{
						INDY_CORE_ERROR("Cannot subscribe with delegate ID [{0}]. Delegate with that ID already exists!");
						return;
					}
				}

				m_Listeners.emplace_back(del);
			};

			void operator-=(const TEventDelegate<EventType>& del)
			{
				if (m_Listeners.empty())
					return;

				// Find the delegate to remove
				size_t deleteIndex = SIZE_MAX;
				for (const auto& listener : m_Listeners)
				{
					++deleteIndex;
					if (listener.id == del.id)
						break;
				}

				// Bail if the delegate was not found
				if (deleteIndex == SIZE_MAX)
				{
					INDY_CORE_ERROR("Could not remove event delegate with id [{0}]. Delegate does not exist!", del.id);
					return;
				}

				// No need to swap if there is only one listener
				if (m_Listeners.size() == 1)
				{
					m_Listeners.pop_back();
					return;
				}

				// Swap the target element and the last element
				TEventDelegate<EventType> temp = m_Listeners[deleteIndex];
				m_Listeners[deleteIndex] = m_Listeners.back();
				m_Listeners[m_Listeners.size() - 1] = temp;

				// Remove last element (which should now be the target
				m_Listeners.pop_back();
			};

		private:
			uint32_t delegateCount = 0;
			std::vector<TEventDelegate<EventType>> m_Listeners;
		};
	}
}