module;

#include <memory>
#include <string>

export module Indy.Input:Layer;

import Indy.Layers;

import :DeviceManager;

export
{
	namespace Indy
	{
		enum class ICL_Action : uint8_t { None = 0xFF, Update = 0x00, CreateLayout, CreateDevice, WatchControl };

		struct ICL_InputEvent : ILayerEvent
		{
			ICL_Action action = ICL_Action::None;
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