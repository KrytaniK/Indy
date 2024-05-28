module;

#include <vector>
#include <memory>
#include <string>

export module Indy.Layers:Layer;

import Indy.Events;

export
{
	namespace Indy
	{
		struct LayerData {}; // Tag structure for complex layer data. This is meant to be derived by layers to enforce layer-specific actions.

		// Base Layer Event Interface
		struct LayerEvent : IEvent
		{
			std::string targetLayer; // A string identifier for layer classifications. Specifies the target layer.
			LayerData* layerData; // Empty tag pointer for storing complex layer data.
		};

		// Base Layer Action Interface. Used for defining layer-specific actions for handling layer data given by layer events.
		class LayerAction
		{
		public:
			virtual ~LayerAction() = default;

			virtual void Execute(LayerData* layerData) = 0;
		};

		// Base Layer Interface
		class Layer
		{
		public:
			virtual ~Layer() = default;

		public:
			virtual void onAttach() = 0;
			virtual void onDetach() = 0;
			virtual void Update() = 0;

		private:
			virtual void onEvent(LayerEvent* event) = 0;

		protected:
			std::vector<std::unique_ptr<LayerAction>> m_LayerActions;
		};
	}
}