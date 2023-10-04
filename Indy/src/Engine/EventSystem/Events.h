#pragma once

#include "../Core/Core.h"
#include "../Core/Log.h"

#include "EventContext.h"

#include <vector>

/*
	The Events namespace is broken into three key components:
		- Event Contexts
		- Event Callbacks
		- Events

	Event contexts give Events meaning. They are responsible
		for managing callbacks and defining how events are dispatched.
		As such, these contexts define event "scopes". Events never
		leave the scope of a given context. To bypass this behavior,
		events must be dispatched again, into the intended scope.
		*Note: Dispatching to the same context will force an infinite loop,
			causing the program to crash. This is an intended oversight,
			as the definition for dispatching events is handled per context.

	Event Callbacks are arbitrary functions that take in an event as 
		their only parameter. Since the signature for member functions
		is different from global functions and lambdas, whenever you
		need to bind a member function to an event context, the "instance"
		of the class containing the function. 

	Events are basic data structures for handling events from within callbacks.
*/

namespace Engine::Events {

	class EventManager;

	// Binds a static or global function to an event context (Also works with lambdas)
	template<typename T_Context, typename T_EventCallbackFunc>
	static EventHandle Bind(const T_EventCallbackFunc& callback)
	{
		return EventManager::Get().Bind<T_Context>(callback);
	}

	// Binds a class/struct member function to an event context
	template<typename T_Class, typename T_Context>
	static EventHandle Bind(T_Class* instance, void (T_Class::*callback)(Event& event))
	{
		return EventManager::Get().Bind<T_Context>([instance, callback](Event& event) { (instance->*callback)(event); });
	}

	template<typename T_Context>
	static bool UnBind(const EventHandle& handle)
	{
		return EventManager::Get().UnBind<T_Context>(handle);
	}

	// Dispatches an event, scoped to the given context
	template<typename T_Context>
	static void Dispatch(Event& event)
	{
		EventManager::Get().Dispatch<T_Context>(event);
	}

	/* Event Manager serves as a "container" for event contexts. This is required,
		since Event Contexts are suppose to be static in nature. They can't live
		in the global scope, as the would only exist for that translation unit,
		thus creating a new instance of a context every time an event is bound.
		By having this "container" singleton class, all of the contexts are managed and
		stay present, so no new instances are created.
	*/
	class EventManager
	{
	private:
		template<typename T_Context>
		static inline T_Context s_Context;

	public:
		static EventManager& Get()
		{
			static EventManager instance;
			return instance;
		}

	public:
		template<typename T_Context, typename T_EventCallbackFunc>
		EventHandle Bind(const T_EventCallbackFunc& callback)
		{
			INDY_CORE_TRACE("[EventManager] Binding Event...");
			return s_Context<T_Context>.addCallback(callback);
		}

		template<typename T_Context>
		bool UnBind(const EventHandle& handle)
		{
			return s_Context<T_Context>.removeCallback(handle);
		}

		// Dispatches an event, scoped to the given context
		template<typename T_Context>
		void Dispatch(Event& event)
		{
			s_Context<T_Context>.dispatch(event);
		}
	};
}