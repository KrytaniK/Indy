#pragma once

#include "Engine/Core/LogMacros.h"

#include "EventManager.h"

/* Currently known issues:
*	- Lifetime Management: Events' ownership is never transferred outside of the scope they're created in. This is an issue
*		because they could potentially live in memory until the Event Manager reaches the end of its own lifecycle.
*	- Additionally, event data must be exclusively stored as a void*, and recast to the original type when it's needed. This
*		is problematic because any callback can edit the data completely and forward that data on to the next callback. This
*		might be intended for some strangely abstract use cases, but even then, I think a better solution exists.
*	- There is no way to track event handles. Callbacks always live in the event manager until they are cleaned up at the end
*		of the program's lifecycle.
*	- The storage of event callbacks via the use of a string map is problematic. There will always be naming conflicts, and events
*		can't be properly handled. If user's aren't careful, they could very easily dispatch events into spaces where the event
*		isn't relevant at all, potentiall introducing breaking behavior.
*/


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