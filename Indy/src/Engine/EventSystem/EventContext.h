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
		ApplicationUpdate = 0, ApplicationShutdown,
		WindowClose, WindowResize, WindowFocus, WindowLoseFocus, WindowMove,
		MouseMove, MouseButton,
		Scroll,
		Keyboard
	};

	struct Event
	{
		EventType type;

		void StopPropagation() { this->b_Propagates = false; };
		bool ShouldPropagate() { return this->b_Propagates; };

	private:
		bool b_Propagates = true;

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

