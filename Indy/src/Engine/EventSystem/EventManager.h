#pragma once

#include "EventContainer.h"
#include "EventHandle.h"

/* Todo: Make this thread-safe
		Currently, all events are considered to be "blocking". They must
		be delt with as soon as they are invoked and not later. A better alternative
		would be to make a multi-threaded solution, or to use some sort
		of queue that handles events at an appropriate time.
*/

/* This class is meant to be as flexible as possible when it comes to event creation and dispatching.
*   The goal is to be able to create events for any scenario without the need to explicity define enums
*	or derive classes. Instead, you can create a struct to represent your event with only the data 
*	you need the event to process, and attach methods to operate on that event.
*/

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
	template<typename T_Event_Type, typename EventCallback_Fn>
	EventHandle AddEventListener(const EventCallback_Fn& callback)
	{
		return sI_EventContainer<T_Event_Type>.addCallback(callback);
	}

	// Overload for non-static class member function pointers
	template<typename T_Event_Type, typename T_Class_Instance, typename EventCallback_Fn>
	EventHandle AddEventListener(const T_Class_Instance& instance, const EventCallback_Fn& callback)
	{
		return sI_EventContainer<T_Event_Type>.addCallback([instance, callback](T_Event_Type event) { (instance->*callback)(event); });
	}

	bool RemoveEventListener(EventHandle& handle)
	{
		return handle.container->removeCallback(handle);
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