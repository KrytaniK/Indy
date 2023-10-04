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
		
		virtual void onUpdate() = 0;

	protected:
		virtual void onEvent(Events::Event& event) = 0;

	};
}