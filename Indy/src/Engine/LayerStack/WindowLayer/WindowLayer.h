#pragma once

#include "../Layer.h"

#include "Engine/Core/Window.h"

namespace Engine
{
	class WindowLayer : public Layer
	{
	public:
		virtual void onAttach() override;
		virtual void onDetach() override;
		virtual void Update() override;

	protected:
		virtual void onRequestWindow(Event& event);

	private:
		std::unique_ptr<Window> m_Window;
		uint8_t m_WindowCount;
	};
}
