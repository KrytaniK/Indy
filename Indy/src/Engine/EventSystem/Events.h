#pragma once

#include "../Core/Core.h"

#include <vector>
#include <functional>

/* Todo: Make this thread-safe
		
		Currently, all events are considered to be "blocking". They must
		be delt with as soon as they are invoked and not later. A better alternative
		would be to make a multi-threaded solution, or to use some sort
		of queue that handles events at an appropriate time.
*/

namespace Events
{
	struct EventHandle
	{
		uint32_t event_id;
	};

	template<typename T_Event_Type, typename T_Callback_Function>
	static inline EventHandle Bind(T_Callback_Function callback)
	{
		return EventManager::GetInstance().AddEventListener<T_Event_Type>(callback);
	}
	
	template<typename T_Event_Type, typename T_Class_Instance, typename T_Callback_Function>
	static inline EventHandle Bind(T_Class_Instance instance, T_Callback_Function callback)
	{
		return EventManager::GetInstance().AddEventListener<T_Event_Type>(instance, callback);
	}

	template<typename T_Event_Type>
	static inline bool UnBind(EventHandle handle)
	{
		return EventManager::GetInstance().RemoveEventListener<T_Event_Type>(handle);
	}

	template<typename T_Event_Type>
	void Dispatch(T_Event_Type event)
	{
		EventManager::GetInstance().DispatchEvent<T_Event_Type>(event);
	}

	// Template container for storing event callbacks based on the event type.
	template<typename T_Event_Type>
	struct EventContainer
	{
		typedef std::function<void(T_Event_Type)> Callback_Function;

		std::vector<Callback_Function> callbacks;
		std::vector<uint32_t> free_callbacks;

		EventHandle addCallback(Callback_Function callback)
		{
			bool b_HasFreeCallback = !free_callbacks.empty();

			// Replace free callback indexed first.
			uint32_t free_index = b_HasFreeCallback ? free_callbacks.back() : callbacks.size();
			EventHandle handle{ free_index };

			if (b_HasFreeCallback)
				free_callbacks.pop_back();

			callbacks.emplace_back(callback);

			return handle;
		}

		bool removeCallback(EventHandle handle)
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

		void invokeCallbacks(T_Event_Type& event)
		{
			for (Callback_Function &callback : callbacks)
			{
				if (callback == nullptr)
					continue;

				callback(event);
			}
		}
	};

	class EventManager
	{
	private:

		// Template container for storing event callbacks based on the event type.
		template<typename T_Event_Type>
		static inline EventContainer<T_Event_Type> sI_EventContainer;

	public:

		// Managed Singleton Instance.
		static EventManager& GetInstance()
		{
			static EventManager instance;
			return instance;
		}

		// Event Registering for regular function pointers and "static" class function pointers
		template<typename T_Event_Type, typename T_Callback_Function>
		EventHandle AddEventListener(T_Callback_Function callback)
		{
			return sI_EventContainer<T_Event_Type>.addCallback(callback);
		}
		
		// Overload for non-static class member function pointers
		template<typename T_Event_Type, typename T_Class_Instance, typename T_Callback_Function>
		EventHandle AddEventListener(T_Class_Instance instance, T_Callback_Function callback)
		{
			return sI_EventContainer<T_Event_Type>.addCallback([instance, callback](T_Event_Type event) { (instance->*callback)(event); });
		}

		template<typename T_Event_Type>
		bool RemoveEventListener(EventHandle handle)
		{
			return sI_EventContainer<T_Event_Type>.removeCallback(handle);
		}

		template<typename T_Event_Type>
		void DispatchEvent(T_Event_Type& event)
		{
			sI_EventContainer<T_Event_Type>.invokeCallbacks(event);
		}

	private:
		EventManager() {}; // Disable Constructor
		EventManager(const EventManager& other) = delete; // Disable Copy Constructor

		void operator=(EventManager&) = delete; // Disable Assignment Operator
	};
}