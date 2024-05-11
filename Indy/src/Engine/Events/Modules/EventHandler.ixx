module;

#include "Engine/Core/LogMacros.h"
#include <cstdint>
#include <vector>
#include <functional>

export module Indy_Core_EventHandler;

export import :EventDelegate;

// TODO: Fix EventDelegate lifetime issues

export
{
	namespace Indy
	{
		struct IEvent
		{
			bool bubbles = true;
			bool propagates = true;
		};

		/*class EventHandler
		{
		public:
			void Notify()
			{

			}

			std::shared_ptr<EventDelegate> Subscribe(EventDelegate del)
			{
				del.id = delegateCount++;
				std::shared_ptr<EventDelegate> eventDelegate = std::make_shared<EventDelegate>(del);

				m_Listeners.emplace_back(eventDelegate);
				return eventDelegate;
			}

			std::shared_ptr<EventDelegate> Subscribe(const std::function<void()>& callback) 
			{
				std::shared_ptr<EventDelegate> eventDelegate = std::make_shared<EventDelegate>(delegateCount++);
				eventDelegate->Bind(callback);

				m_Listeners.emplace_back(eventDelegate);
				return eventDelegate;
			};

			template<class C>
			std::shared_ptr<EventDelegate> Subscribe(const C* instance, const void(C::* callback)()) 
			{
				if (!instance)
				{
					INDY_CORE_INFO("Could not bind member function to event delegate. Instance is null.");
					return;
				}

				if (!callback)
				{
					INDY_CORE_INFO("Could not bind member function to event delegate. Invalid callback.");
					return;
				}

				std::shared_ptr<EventDelegate> eventDelegate = std::make_shared<EventDelegate>(delegateCount++);
				eventDelegate->Bind(instance, callback);

				m_Listeners.emplace_back(eventDelegate);
				return eventDelegate;
			};

			void UnSubscribe(const uint32_t& delegateID)
			{
				if (m_Listeners.size() == 0)
					return;

				if (m_Listeners.size() == 1 && m_Listeners[0]->id == delegateID)
				{
					m_Listeners.pop_back();
					return;
				}

				size_t removeIndex = SIZE_MAX;
				for (const auto& listener : m_Listeners)
				{
					++removeIndex;

					if (listener->id == delegateID)
						break;
				}

				if (removeIndex == SIZE_MAX)
					return;

				std::shared_ptr<EventDelegate> temp = m_Listeners[removeIndex];
				m_Listeners[removeIndex] = m_Listeners[m_Listeners.size() - 1];
				m_Listeners[m_Listeners.size() - 1] = temp;

				m_Listeners.pop_back();
			}

			void operator+=(EventDelegate del)
			{
				Subscribe(del);
			}

		private:
			uint32_t delegateCount = 0;
			std::vector<std::shared_ptr<EventDelegate>> m_Listeners;
		};*/

		class EventHandler
		{
		public:
			void Notify()
			{
				for (const std::shared_ptr<EventDelegate>& listener : m_Listeners)
				{
					if (listener)
						listener->Exec();
				}
			};

			std::shared_ptr<EventDelegate> Subscribe(EventDelegate& del)
			{
				// Always set the id.
				// Ensures all delegate IDs are 'unique'
				del.id = delegateCount++;

				// Create a shared pointer out of the delegate
				std::shared_ptr<EventDelegate> sharedDelegate = std::shared_ptr<EventDelegate>(&del);

				m_Listeners.emplace_back(sharedDelegate);
				return sharedDelegate;
			}

			void Subscribe(std::shared_ptr<EventDelegate>& del)
			{
				// Always set the id.
				// Ensures all delegate IDs are 'unique'
				del->id = delegateCount++;

				m_Listeners.emplace_back(del);
			}

			std::shared_ptr<EventDelegate> Subscribe(const std::function<void()>& callback)
			{
				std::shared_ptr<EventDelegate> sharedDelegate = std::make_shared<EventDelegate>(delegateCount++);
				sharedDelegate->Bind(callback);

				m_Listeners.emplace_back(sharedDelegate);
				return sharedDelegate;
			}

			template<class C>
			std::shared_ptr<EventDelegate> Subscribe(C* instance, void(C::* callback)())
			{
				std::shared_ptr<EventDelegate> sharedDelegate = std::make_shared<EventDelegate>(delegateCount++);
				sharedDelegate->Bind<C>(instance, callback);

				m_Listeners.emplace_back(sharedDelegate);
				return sharedDelegate;
			}

			void UnSubscribe(const EventDelegate& del)
			{ 
				if (m_Listeners.empty())
					return;

				if (m_Listeners.size() == 1 && m_Listeners[0]->id == del.id)
				{
					m_Listeners.pop_back();
					return;
				}

				// Find the delegate to remove
				size_t deleteIndex = SIZE_MAX;
				for (const auto& listener : m_Listeners)
				{
					++deleteIndex;
					if (listener->id == del.id)
						break;
				}

				// Bail if the delegate was not found
				if (deleteIndex == SIZE_MAX)
				{
					INDY_CORE_ERROR("Could not remove event delegate with id [{0}]. Delegate does not exist!", del.id);
					return;
				}

				// Swap the target element and the last element
				std::shared_ptr<EventDelegate> temp = m_Listeners[deleteIndex];
				m_Listeners[deleteIndex] = m_Listeners.back();
				m_Listeners[m_Listeners.size() - 1] = temp;

				// Remove last element (which should now be the target
				m_Listeners.pop_back();
			}

			void operator()()
			{
				this->Notify();
			}

			void operator+=(const std::function<void()>& callback)
			{
				this->Subscribe(callback);
			}

			void operator+=(EventDelegate& del)
			{
				this->Subscribe(del);
			};

			void operator-=(const EventDelegate& del)
			{
				this->UnSubscribe(del);
			};

		private:
			uint32_t delegateCount = 0;
			std::vector<std::shared_ptr<EventDelegate>> m_Listeners;
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

			TEventDelegate<EventType> operator+=(const std::function<void(EventType*)>& callback)
			{
				return Subscribe(callback);
			}

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