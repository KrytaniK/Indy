module;

#include <memory>
#include <string>

export module Indy_Core_InputLayer;

export import :Actions;

import Indy_Core_LayerStack;
import Indy_Core_Input;

export
{
	namespace Indy
	{
		enum class InputLayerAction : uint8_t { None = 0xFF, Update = 0x00, CreateDevice, CreateLayout, WatchControl };

		struct InputLayerEvent : ILayerEvent
		{
			InputLayerAction action = InputLayerAction::None;
		};

		class InputLayer : public ILayer
		{
		public:
			virtual void onAttach() override;
			virtual void onDetach() override;
			virtual void Update() override;

		private:
			virtual void onEvent(ILayerEvent* event) override;

		private:
			std::shared_ptr<DeviceManager> m_DeviceManager;
		};
	}
}