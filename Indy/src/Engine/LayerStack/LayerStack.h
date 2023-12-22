#pragma once

#include "Layer.h"

namespace Engine
{
	class LayerStack
	{
	public:
		static std::vector<Layer*> s_Layers;

		static void Cleanup();

		static void Update();

		static void Push(Layer* layer);
		static void PushOverlay(Layer* layer);

	private:
		static uint32_t s_LayerIndex;
	};
}