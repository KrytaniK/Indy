#include <Engine/Core/LogMacros.h>

#include <memory>
#include <functional>

import Indy.Events;

namespace Indy
{
	EventHandler::EventHandler()
	{
		m_DelegateCount = 0;
		m_Listeners.reserve(25);
	}

	void EventHandler::Notify(IEvent* event)
	{
		if (!event->bubbles)
		{
			size_t i = 0;
			while (i != m_Listeners.size())
			{
				m_Listeners[i++]->Execute(event);

				if (!event->propagates)
					return;
			}
		}
		else
		{
			size_t i = m_Listeners.size();
			while (i != 0)
			{
				m_Listeners[--i]->Execute(event);

				if (!event->propagates)
					return;
			}
		}
	}

	void EventHandler::Notify()
	{
		size_t i = 0;
		while (i != m_Listeners.size())
			m_Listeners[i++]->Execute();
	}

	std::shared_ptr<EventDelegate> EventHandler::Subscribe(const std::function<void()>& callback)
	{
		std::shared_ptr<EventDelegate> sharedDelegate = std::make_shared<EventDelegate>(m_DelegateCount++);
		sharedDelegate->Bind(callback);

		m_Listeners.emplace_back(sharedDelegate);
		return sharedDelegate;
	}

	std::shared_ptr<EventDelegate> EventHandler::Subscribe(EventDelegate& eventDelegate)
	{
		eventDelegate.m_ID = m_DelegateCount++;
		std::shared_ptr<EventDelegate> sharedDelegate = std::make_shared<EventDelegate>(eventDelegate);

		m_Listeners.emplace_back(sharedDelegate);
		return sharedDelegate;
	}

	void EventHandler::Subscribe(std::shared_ptr<EventDelegate>& eventDelegate)
	{
		eventDelegate->m_ID = m_DelegateCount++;
		m_Listeners.emplace_back(eventDelegate);
	}

	void EventHandler::UnSubscribe(const EventDelegate& eventDelegate)
	{
		if (m_Listeners.empty())
			return;

		if (m_Listeners.size() == 1 && m_Listeners[0]->m_ID == eventDelegate.m_ID)
		{
			m_Listeners.pop_back();
			return;
		}

		// Find the delegate to remove
		size_t deleteIndex = SIZE_MAX;
		for (const auto& listener : m_Listeners)
		{
			++deleteIndex;
			if (listener->m_ID == eventDelegate.m_ID)
				break;
		}

		// Bail if the delegate was not found
		if (deleteIndex == SIZE_MAX)
		{
			INDY_CORE_ERROR("Could not remove event delegate with id [{0}]. Delegate does not exist!", eventDelegate.m_ID);
			return;
		}

		// Swap the target element and the last element
		std::shared_ptr<EventDelegate> temp = m_Listeners[deleteIndex];
		m_Listeners[deleteIndex] = m_Listeners.back();
		m_Listeners[m_Listeners.size() - 1] = temp;

		// Remove last element (which should now be the target
		m_Listeners.pop_back();
	}
}