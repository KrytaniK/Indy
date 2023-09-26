#pragma once

#include "EventHandle.h"

#include <vector>
#include <functional>

/* It might be worth noting...
*	This implementation could mean longer compile times as the number of event types grow with the application.
*	To work around this, moving away from template structs/classes to storing event types as enums and
*		storing relative callbacks in an ordered/unordered map might be a better approach. For now,
*		I will leave this as is. However, it is likely to change.
*/

template<typename T_Event_Type>
struct EventContainer
{
	using EventCallback_Fn = std::function<void(T_Event_Type&)>;

	std::vector<EventCallback_Fn> callbacks;
	std::vector<uint32_t> free_callbacks;

	// Subscribes to an Event, returning a new EventHandle
	EventHandle addCallback(const EventCallback_Fn& callback)
	{
		bool b_HasFreeIndex = !free_callbacks.empty();

		// Replace free callback indexed first.
		uint32_t free_index = b_HasFreeIndex ? free_callbacks.back() : callbacks.size();
		EventHandle handle{ free_index };

		if (b_HasFreeIndex)
			free_callbacks.pop_back();

		callbacks.emplace_back(callback);

		return handle;
	}

	// Removes an event, given an EventHandle. Returns true if the operation was successful
	bool removeCallback(const EventHandle& handle)
	{
		if (callbacks[handle.event_id] == nullptr)
		{
			free_callbacks.emplace_back(handle.event_id);
			return false;
		}

		callbacks[handle.event_id] = nullptr;
		free_callbacks.emplace_back(handle.event_id);

		return true;
	}

	// Invokes callbacks in reverse order. Callbacks registered first will be invoked last.
	void invokeCallbacks(const T_Event_Type& event) const
	{
		for (auto it = callbacks.rbegin(); it != callbacks.rend(); ++it)
		{
			EventCallback_Fn callback = *it;

			if (callback == nullptr)
				continue;

			callback(event);
		}
	}
};