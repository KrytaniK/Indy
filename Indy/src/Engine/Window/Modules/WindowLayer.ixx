module;

#include <vector>
#include <map>
#include <memory>
#include <queue>

export module Indy_Core_WindowLayer;

export import :Actions;

import Indy_Core_LayerStack;
import Indy_Core_Events;
import Indy_Core_Window;


export
{
	namespace Indy
	{
		enum class WindowLayerAction : uint8_t { None = 0xFF, RequestWindow = 0x00, OpenWindow, DestroyWindow };

		struct WindowLayerEvent : ILayerEvent
		{
			WindowLayerAction action = WindowLayerAction::None;
		};

		class WindowLayer : public ILayer
		{
		public: 

		public:
			virtual void onAttach() override;
			virtual void onDetach() override;
			virtual void Update() override;

		private:
			virtual void onEvent(ILayerEvent* event) override;

		private:
			std::shared_ptr<WindowManager> m_WindowManager;
		};
	}
}