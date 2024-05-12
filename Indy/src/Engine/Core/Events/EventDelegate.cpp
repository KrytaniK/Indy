#include <Engine/Core/LogMacros.h>

#include <functional>

import Indy.Events;

namespace Indy
{
	EventDelegate::EventDelegate(const uint32_t& id)
	{
		m_ID = id;
	}

	void EventDelegate::Execute(IEvent* event)
	{
		if (!m_EventFunc)
		{
			INDY_CORE_WARN("[ID: {0}] No function bound matching signature: std::function<void(IEvent*)>", m_ID);
			return;
		}

		m_EventFunc(event);
	}

	void EventDelegate::Execute()
	{
		if (!m_BaseFunc)
		{
			INDY_CORE_WARN("[ID: {0}] No function bound matching signature: std::function<void()>", m_ID);
			return;
		}

		m_BaseFunc();
	}

	void EventDelegate::Bind(const std::function<void()>& callback)
	{
		m_BaseFunc = callback;
	}
}