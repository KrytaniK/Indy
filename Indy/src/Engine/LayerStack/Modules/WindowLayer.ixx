module;

#include <vector>
#include <map>
#include <memory>

export module Indy_Core_LayerStack:WindowLayer;

import Indy_Core_EventSystem;
import Indy_Core_Window;

import :Layer;

export
{
	namespace Indy
	{
		class WindowLayer : public Layer
		{
		public:
			virtual void onAttach() override;
			virtual void onDetach() override;
			virtual void Update() override;

		private:
			void OnWindowCreate(WindowCreateEvent* event);
			void OnWindowDestroy(WindowDestroyEvent* event);
			void OnRequestWindow(WindowRequestEvent* event);

			uint8_t GenerateWindowID();

		private:
			std::vector<std::unique_ptr<IWindow>> m_Windows;
			std::map<uint8_t, uint8_t> m_WindowIndices;
			uint8_t m_WindowCount = 0;
		};
	}
}