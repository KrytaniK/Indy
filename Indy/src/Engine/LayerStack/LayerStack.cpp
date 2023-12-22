#include "LayerStack.h"

namespace Engine
{
	std::vector<Layer*> LayerStack::s_Layers;
	uint32_t LayerStack::s_LayerIndex;

	void LayerStack::Cleanup()
	{
		for (auto layer = s_Layers.begin(); layer != s_Layers.end(); ++layer)
		{
			(*layer)->onDetach();

			delete *layer;
		}
	}

	void LayerStack::Update()
	{
		for (auto layer : s_Layers)
			layer->Update();
	}

	void LayerStack::Push(Layer* layer)
	{
		s_Layers.insert(s_Layers.begin() + s_LayerIndex, layer);
		layer->onAttach();
	}

	void LayerStack::PushOverlay(Layer* layer)
	{
		s_Layers.emplace_back(layer);
	}
}