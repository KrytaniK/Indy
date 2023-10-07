#pragma once

#include "Engine/EventSystem/Events.h"

namespace Engine
{
	class Layer
	{

	public:
		Layer() {};
		~Layer() {};

	private:
		virtual void onAttach() = 0;
		virtual void onDetach() = 0;

	protected:
		virtual void onUpdate(Event& event) = 0;
		virtual void onEvent(Event& event) = 0;

	};
}