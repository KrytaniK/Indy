#pragma once

#include <functional>
#include <string>

namespace Engine::Events {


	/*
		This is just me, but I absolutely hate event types as an enum. There has to be a cleaner,
		safer, less tedious approach. As the application grows, so too will this enum, and I shiver
		at the thought. FIND SOMETHING BETTER!
	*/

	enum EventType
	{
		ApplicationUpdate = 0,
	};

	struct Event
	{
		// Returns a boolean representation of whether this event should "bubble".
		bool Bubbles() { return this->b_Bubbles; };
		// Sets the internal "bubbles" value for this event.
		void PreventBubbling(bool bubbles) { 
			if (this->b_Terminal) return; // Terminal events MUST bubble.

			this->b_Bubbles = bubbles; 
		}

		// Returns a boolean representation of whether this event should propagate.
		// Returns true by default.
		bool Propagates() { return this->b_Propagates; };
		// Sets the internal propagation value for this event to false.
		void StopPropagation() { this->b_Propagates = false; };

		// Returns a boolean representation of whether this event is considered
		// "terminal". For most cases, this should ONLY be used for triggering
		// application shutdown behavior.
		bool Terminal() { return this->b_Terminal; };

		// Setter for internal event "terminal" boolean. For most cases, this should 
		// ONLY be used for events that trigger application shutdown behavior.
		void IsTerminal(bool isTerminal) { 
			this->b_Terminal = isTerminal;
			this->b_Bubbles = true;
		};

	private:
		/* Event bubbling refers to when an event starts at the
			highest level in the context and propagates to the root.
			
			In the context of an array, this means that an event starts
			at the end of the array and works its way to the start.
			
			By default, events are set to bubble, so if you need an event
			to propagate from the beginning of an array to the end, it needs
			to explicitly be set to false.
		 
			It should be noted that once an event has been dispatched, unless
			explicitly handled by the context, switching event propagation behavior 
			through b_Bubbles does nothing.
		*/
		bool b_Bubbles = true;

		/* Event Propagation refers to whether the event will propagate to the next
			event handler function in the list of event callbacks, managed by the
			EventContext.

			Setting this to false means the event has been handled and should not
			be subsequently acted upon by methods further down or up in the callback
			chain.
		*/
		bool b_Propagates = true;

		/* A Terminal event is one that should be used to trigger application level
			events, such as application shutdown. If this is set to true, the event will
			always bubble, propagating from the highest level down to the root ancestor in
			the event chain. As such, SetBubbles() will do nothing if this is true.
		*/
		bool b_Terminal = false;
	};

	struct EventHandle
	{
		uint32_t event_id;
	};

	struct IEventContext
	{
		using EventCallbackFunc = std::function<void(Event&)>;

		public:
			virtual EventHandle addCallback(const EventCallbackFunc& callback) = 0;
			virtual bool removeCallback(const EventHandle& handle) = 0;
			virtual void dispatch(Event& event) = 0;
	};


}

