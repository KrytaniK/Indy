#pragma once

#include "../Core/Log.h"

#include "EventManager.h"

// Note: Binding currently returns a size_t to resolve compile warnings. Later this will be changed to return an event handle.

namespace Engine::Events {

	// Binds a static or global function to an event context (Also works with lambdas)
	static size_t Bind(const std::string& context, const std::string& type, const std::function<void(Event&)>& callback)
	{
		return EventManager::Get().AddEventListener(context, type, callback);
	}

	// Binds a class/struct member function to an event context
	template<typename T_Class, typename T_CallbackFun>
	static size_t Bind(const std::string& context, const std::string& type, T_Class* instance, T_CallbackFun callback)
	{
		return EventManager::Get().AddEventListener(context, type, EventManager::ConvertMemFun(instance, callback));
	}

	static bool UnBind(const std::string& context, const std::string& type, const int& index)
	{
		return EventManager::Get().RemoveEventListener(context, type, index);
	}

	// Dispatches an event, scoped to the given context
	static void Dispatch(Event& event)
	{
		EventManager::Get().DispatchEvent(event);
	}
}