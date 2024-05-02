module;

#include <vector>
#include <map>
#include <memory>
#include <queue>

export module Indy_Core:WindowLayer;

import Indy_Core_Events;
import Indy_Core_Window;

import :Layer;

export
{
	namespace Indy
	{
		class WindowLayer : public ILayer
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
			void DestroyWindow(uint8_t id);

		private:
			std::vector<std::unique_ptr<IWindow>> m_Windows;
			std::queue<uint8_t> m_WindowDeleteQueue;
			std::map<uint8_t, uint8_t> m_WindowIndices;
			uint8_t m_WindowCount = 0;
		};
	}
}