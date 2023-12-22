#pragma once

#include "Engine/EventSystem/Events.h"

namespace Engine
{
	class Layer
	{
	public:
		virtual void onAttach() = 0;
		virtual void onDetach() = 0;
		virtual void Update() = 0;

	};
}