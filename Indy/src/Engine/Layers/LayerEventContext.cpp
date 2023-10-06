#include "LayerEventContext.h"

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
		if (event.Bubbles())
		{
			auto _iterator = m_Callbacks.rbegin();
			auto _iterator_end = m_Callbacks.rend();

			for (_iterator; _iterator != _iterator_end; _iterator++)
			{
				if (!event.Propagates()) return;

				EventCallbackFunc callback = *_iterator;
				if (callback == nullptr) continue;
				else callback(event);
			}
		} 
		else
		{
			auto _iterator = m_Callbacks.begin();
			auto _iterator_end = m_Callbacks.end();

			for (_iterator; _iterator != _iterator_end; _iterator++)
			{
				if (!event.Propagates()) return;

				EventCallbackFunc callback = *_iterator;
				if (callback == nullptr) continue;
				else callback(event);
			}
		}
	}
}