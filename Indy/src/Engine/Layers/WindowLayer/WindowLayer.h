#pragma once

#include "../Layer.h"

#include "Engine/Core/Window.h"

namespace Engine
{
	class WindowLayer : public Layer
	{
	public:
		WindowLayer();
		~WindowLayer();

	private:
		virtual void onAttach() override;
		virtual void onDetach() override;

	protected:
		virtual void onUpdate(Event& event) override;

		virtual void onRequestWindow(Event& event);

	private:
		std::unique_ptr<Window> m_Window;
	};
}
