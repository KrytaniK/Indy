module;

#include <vector>
#include <map>
#include <memory>
#include <queue>

export module Indy.Window:Layer;

export import :LayerActions;

import Indy.Layers;
import Indy.Events;
import Indy.WindowManager;


export
{
	namespace Indy
	{
		enum class WindowLayerAction : uint8_t { None = 0xFF, RequestWindow = 0x00, OpenWindow, DestroyWindow };

		struct ICL_WindowEvent : ILayerEvent
		{
			WindowLayerAction action = WindowLayerAction::None;
		};

		class WindowLayer : public ILayer
		{
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