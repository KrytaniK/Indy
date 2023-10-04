#pragma once

#include "Layer.h"
#include "Engine/EventSystem/Events.h"

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

		virtual void onEvent(Events::Event& event) override;

		virtual void onUpdate() override;


	private:
		Events::EventHandle m_EventHandle;
		std::unique_ptr<Window> m_Window;
	};
}
