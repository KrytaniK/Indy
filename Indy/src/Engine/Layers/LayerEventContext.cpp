#include "LayerEventContext.h"

/*
	To Do: Fix Callback Adding and Removal
*/

namespace Engine
{
	Events::EventHandle LayerEventContext::addCallback(const EventCallbackFunc& callback)
	{
		bool b_HasFreeIndex = !m_FreeCallbacks.empty();

		// Replace free callback indexed first.
		/*uint32_t free_index = b_HasFreeIndex ? m_FreeCallbacks.back() : m_Callbacks.end();*/
		Events::EventHandle handle{ 0 };

		// Remove the last known free index, since it's now being used.
		if (b_HasFreeIndex)
			m_FreeCallbacks.pop_back();

		m_Callbacks.emplace(m_Callbacks.end(), callback);

		//INDY_CORE_INFO("Testing New Event...");
		/*if (m_Callbacks[free_index] == nullptr)
		{
			if (callback == nullptr)
				INDY_CORE_ERROR("Callback was not stored properly!");
			else
				INDY_CORE_ERROR("Callback param was null!");
		}*/

		return handle;
	}

	bool LayerEventContext::removeCallback(const Events::EventHandle& handle)
	{
		// Callback wasn't found
		if (m_Callbacks[handle.event_id] == nullptr)
		{
			m_FreeCallbacks.emplace_back(handle.event_id);
			return false;
		}

		// "Free" callback from memory and store the index for later use.
		m_Callbacks[handle.event_id] = nullptr;
		m_FreeCallbacks.emplace_back(handle.event_id);

		return true;
	}

	void LayerEventContext::dispatch(Events::Event& event)
	{
		// Dispatch Events from the lowest layer upward (Application -> Window)
		for (auto it = m_Callbacks.begin(); it != m_Callbacks.end(); it++)
		{
			if (!event.ShouldPropagate()) return;

			EventCallbackFunc callback = *it;

			if (callback == nullptr)
			{
				continue;
			}

			callback(event);
		}
	}
}