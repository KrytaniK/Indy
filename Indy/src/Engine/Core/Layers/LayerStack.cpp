#include <Engine/Core/LogMacros.h>

import Indy.Layers;

namespace Indy
{
	LayerStack::LayerStack()
		: m_OverlayStartIndex(0)
	{}

	LayerStack::~LayerStack()
	{
		for (size_t i = 0; i < m_Layers.size(); i++)
		{
			m_Layers.at(i)->onDetach();
		}
	}

	void LayerStack::Update()
	{
		for (const auto& layer : m_Layers)
			if (layer)
				layer->Update();
	}

	void LayerStack::PushLayer(const std::shared_ptr<Layer> layer)
	{
		m_Layers.insert(m_Layers.begin() + m_OverlayStartIndex++, layer);
		layer->onAttach();
	}

	void LayerStack::PushOverlay(const std::shared_ptr<Layer> overlay)
	{
		m_Layers.emplace_back(overlay);
		overlay->onAttach();
	}
}