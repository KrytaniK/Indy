#pragma once

#include "../Core/Core.h"
#include "EventManager.h"

/// <summary>
/// An "ease-of-use" collection of methods for registering and dispatching global events.
/// </summary>
namespace Events {

	struct EventBase {};

	template<typename T_Event_Type, typename T_Callback_Function>
	static inline EventHandle Bind(const T_Callback_Function& callback)
	{
		return EventManager::GetInstance().AddEventListener<T_Event_Type>(callback);
	}
	
	template<typename T_Event_Type, typename T_Class_Instance, typename T_Callback_Function>
	static inline EventHandle Bind(const T_Class_Instance& instance, const T_Callback_Function& callback)
	{
		return EventManager::GetInstance().AddEventListener<T_Event_Type>(instance, callback);
	}

	static inline bool UnBind(EventHandle& handle)
	{
		return EventManager::GetInstance().RemoveEventListener(handle);
	}

	template<typename T_Event_Type>
	void Dispatch(T_Event_Type& event)
	{
		EventManager::GetInstance().DispatchEvent<T_Event_Type>(event);
	}
}