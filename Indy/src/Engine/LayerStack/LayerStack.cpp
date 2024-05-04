#include "Engine/Core/LogMacros.h"

import Indy_Core_LayerStack;

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
		for (size_t i = 0; i < m_Layers.size(); i++)
		{
			m_Layers.at(i)->Update();
		}
	}

	void LayerStack::PushLayer(const std::shared_ptr<ILayer> layer)
	{
		m_Layers.insert(m_Layers.begin() + m_OverlayStartIndex++, layer);
		layer->onAttach();
	}

	void LayerStack::PushOverlay(const std::shared_ptr<ILayer> overlay)
	{
		m_Layers.emplace_back(overlay);
		overlay->onAttach();
	}
}