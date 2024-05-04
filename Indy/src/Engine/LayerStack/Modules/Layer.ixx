module;

#include <vector>
#include <memory>
#include <string>

export module Indy_Core_LayerStack:Layer;

import Indy_Core_Events;

export
{
	namespace Indy
	{
		struct ILayerData {}; // Tag structure for complex layer data. This is meant to be derived by layers to enforce layer-specific actions.

		// Base Layer Event Interface
		struct ILayerEvent : IEvent
		{
			std::string targetLayer; // A string identifier for layer classifications. Specifies the target layer.
			ILayerData* layerData; // Empty tag pointer for storing complex layer data.
		};

		// Base Layer Action Interface. Used for defining layer-specific actions for handling layer data given by layer events.
		class ILayerAction
		{
		public:
			~ILayerAction() {};

			virtual void Execute(ILayerData* layerData) = 0;
		};

		// Base Layer Interface
		class ILayer
		{
		public:
			virtual ~ILayer()
			{
				for (const auto& handle : m_EventHandles)
					EventManagerCSR::RemoveEventListener(handle);
			};

		public:
			virtual void onAttach() = 0;
			virtual void onDetach() = 0;
			virtual void Update() = 0;

		private:
			virtual void onEvent(ILayerEvent& event) = 0;

		protected:
			std::vector<IEventHandle> m_EventHandles;
			std::vector<std::unique_ptr<ILayerAction>> m_LayerActions;
		};
	}
}