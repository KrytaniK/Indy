#pragma once

#include "Engine/EventSystem/Events.h"

namespace Engine
{
	class LayerEventContext : Events::IEventContext
	{
		using EventCallbackFunc = std::function<void(Events::Event&)>;

	private:
		std::vector<EventCallbackFunc> m_Callbacks;
		std::vector<uint32_t> m_FreeCallbacks;

	public:
		virtual Events::EventHandle addCallback(const EventCallbackFunc& callback) override;

		virtual bool removeCallback(const Events::EventHandle& handle) override;

		virtual void dispatch(Events::Event& event) override;
	};
}