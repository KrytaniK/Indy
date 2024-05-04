module;

#include <cstdint>
#include <vector>
#include <memory>
#include <string>

export module Indy_Core:LayerStack;

import Indy_Core_Events;

export
{
	namespace Indy
	{
		constexpr uint8_t INDY_CORE_LAYER_APPLICATION = 0x00;
		constexpr uint8_t INDY_CORE_LAYER_WINDOW = 0x01;
		constexpr uint8_t INDY_CORE_LAYER_INPUT = 0x02;
		constexpr uint8_t INDY_CORE_LAYER_GUI = 0x03;

		// ILayerData - Data structure meant to be derived by layers to define complex layer event data that might be specific to a layer.
		struct ILayerData {
			void* data = nullptr; // Pointer to pass raw event data. 
		};

		struct LayerEvent : IEvent
		{
			uint8_t target = 0x00; // A 1-byte identifier (0-255) for layer event classifications. Specifies the target layer. (e.g., Application, Window, Input, Gui)
			uint8_t action = 0x00; // A 1-byte identifier (0-255) for layer event types. Specifies how the layer should respond to this event. (e.g., Request, Notify, Create, Destroy)
			ILayerData* data; // An event pointer to pass derived event data without sacrificing type information.
		};

		class ILayer
		{
		public:
			virtual void onAttach() = 0;
			virtual void onDetach() = 0;
			virtual void Update() = 0;

		private:
			virtual void onEvent(LayerEvent& event) = 0;

		protected:
			std::vector<IEventHandle> m_EventHandles;
		};

		class LayerStack
		{
		public:
			LayerStack();
			~LayerStack();

			void Update();
			void PushLayer(const std::shared_ptr<ILayer> layer);
			void PushOverlay(const std::shared_ptr<ILayer> overlay); // should likely derive an Overlay class from Layer

		private:
			std::vector<std::shared_ptr<ILayer>> m_Layers;
			uint32_t m_OverlayStartIndex;
		};
	}
}