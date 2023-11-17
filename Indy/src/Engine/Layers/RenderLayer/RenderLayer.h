#pragma once

#include "Engine/Layers/Layer.h"
#include "Engine/Renderer/Renderer.h"

#include <memory>

namespace Engine
{
	class RenderLayer : public Layer
	{
	public:
		RenderLayer();
		~RenderLayer();

	private:
		virtual void onAttach() override;
		virtual void onDetach() override;

		void onRender(Event& event);

	protected:
		virtual void onUpdate(Event& event) override;
		virtual void onEvent(Event& event) override;
	};
}