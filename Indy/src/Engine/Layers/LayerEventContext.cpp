#include "LayerEventContext.h"

/*
	To Do: Fix Callback Adding and Removal
*/

namespace Engine
{
	// Adds an event callback to the "layer" context, effectively attaching a layer.
	// Returns -1 if the callback could not be added.
	Events::EventHandle LayerEventContext::addCallback(const EventCallbackFunc& callback)
	{
		bool b_HasFreeIndex = !m_FreeCallbacks.empty();

		// Replace free callback indexed first.
		uint32_t free_index = -1;

		// Remove the last known free index, since it's now being used.
		if (b_HasFreeIndex) {
			free_index = m_FreeCallbacks.back();
			m_FreeCallbacks.pop_back();
		} else {
			free_index = m_Callbacks.size();
			m_Callbacks.emplace(m_Callbacks.end(), callback);
		}

		Events::EventHandle handle{ free_index };
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
		auto _iterator_begin = event.Bubbles() ? m_Callbacks.end() : m_Callbacks.begin();
		auto _iterator_end = event.Bubbles() ? m_Callbacks.begin() : m_Callbacks.end();

		// Dispatch Events from the lowest layer upward (Application -> Window)
		for (auto &it = _iterator_begin; it != _iterator_end; it++)
		{
			if (!event.Propagates()) return;

			EventCallbackFunc callback = *it;

			if (callback == nullptr)
			{
				continue;
			}

			callback(event);
		}
	}
}