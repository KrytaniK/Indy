#pragma once

#include "Engine/Core/Log.h"

#include <map>
#include <vector>
#include <functional>
#include <string>

/*

	I want to make it explicitly clear that this approach needs to be battle tested. It should work "flawlessly",
		meaning there should be no immediate drawbacks outside of member function conversion, which I will implement later.
		Using this approach, I should be able to attach event callbacks to specific types within specific callbacks.

	Making this work within a layer system would be very easy. Layers can take in the event
*/

namespace Engine
{
	struct Event
	{
		std::string context = "Application";
		std::string type = "None";
		bool bubbles = true;
		bool propagates = true;

		void* data; // data field for attaching arbitrary event data
	};

	class EventManager
	{
	public:
		static EventManager& Get() // Singleton Instance
		{
			static EventManager instance;
			return instance;
		}

		// This currently doesn't account for empty regions in the vector. To remedy this, a struct should be used that holds both
		//	the event callbacks and the indexes to those free regions, so that we can more easily manage event listeners.

		size_t AddEventListener(const std::string& context, const std::string& eventType, const std::function<void(Event&)>& eventCallbackFunc)
		{
			std::vector<std::function<void(Event&)>>& callbacks = m_ContextMap[context][eventType];

			callbacks.emplace_back(eventCallbackFunc);
			return callbacks.size() - 1;
		}

		bool RemoveEventListener(const std::string& context, const std::string& eventType, const int& index) // Might later take an "event handle" instead of an index
		{
			std::vector< std::function<void(Event&)>>& callbacks = m_ContextMap[context][eventType];

			if (!callbacks[index] || callbacks[index] == nullptr) return false;

			callbacks[index] = nullptr;
			return true;
		}

		void DispatchEvent(Event& event)
		{
			if (event.bubbles)
			{
				auto _itstart = m_ContextMap[event.context][event.type].rbegin();
				auto _itend = m_ContextMap[event.context][event.type].rend();

				for (_itstart; _itstart != _itend; _itstart++)
				{
					std::function<void(Event&)> callback = *_itstart;

					if (callback == nullptr) continue;

					callback(event);
				}
			}
			else
			{
				auto _itstart = m_ContextMap[event.context][event.type].begin();
				auto _itend = m_ContextMap[event.context][event.type].end();

				for (_itstart; _itstart != _itend; _itstart++)
				{
					std::function<void(Event&)> callback = *_itstart;

					if (callback == nullptr) continue;

					callback(event);
				}
			}
		}


		// Utility method for converting member function signature to standard function signature.
		template<typename T_Class>
		static std::function<void(Event&)> ConvertMemFun(T_Class* instance, void(T_Class::* callback)(Event& event))
		{
			return [instance, callback](Event& event) { (instance->*callback)(event); };
		}

	private:
		// This type is a bit verbose. I should probably consider another approach, but this works for now. I have yet to see a performance impact.
		std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::function<void(Event&)>>>> m_ContextMap;
	};
}