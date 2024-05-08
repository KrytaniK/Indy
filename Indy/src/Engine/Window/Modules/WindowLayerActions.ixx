module;

#include <memory>
#include <cstdint>
#include <string>

export module Indy_Core_WindowLayer:Actions;

import Indy_Core_LayerStack;
import Indy_Core_Window;

export
{
	namespace Indy
	{
		// ----- Layer Action Data -----

		// Layer Data for creating windows
		struct ICL_WindowData_Create : ILayerData
		{
			std::string title = "Indy Engine";
			unsigned int width = 1280, height = 760;
			uint8_t id = 0;
			IWindowHandle* handle; // A handle to store the created window.
		};

		// Layer Data for destroying windows
		struct ICL_WindowData_Destroy : ILayerData
		{
			uint8_t index; // Index of the target window
		};

		struct ICL_WindowData_Request : ILayerData
		{
			uint8_t index; // Index of the target window. Used if boolean getActiveWindow is set to false.
			bool getActiveWindow; // If set to true, requests the window that is currently in focus.
			IWindowHandle* handle; // A handle to the requested window.
		};

		// ----- Layer Actions -----

		class ICL_WindowAction_Create : public ILayerAction
		{
		public:
			ICL_WindowAction_Create(WindowManager* windowManager)
				: m_WindowManager(windowManager) {};

			virtual void Execute(ILayerData* layerData) override;

		private:
			WindowManager* m_WindowManager;
		};

		// Layer Action for destroying windows
		class ICL_WindowAction_Destroy : public ILayerAction
		{
		public:
			ICL_WindowAction_Destroy(WindowManager* windowManager)
				: m_WindowManager(windowManager) {};

			virtual void Execute(ILayerData* layerData) override;

		private:
			WindowManager* m_WindowManager;
		};

		// Layer Action for requesting a window
		class ICL_WindowAction_Request : public ILayerAction
		{
		public:
			ICL_WindowAction_Request(WindowManager* windowManager)
				: m_WindowManager(windowManager) {};

			virtual void Execute(ILayerData* layerData) override;

		private:
			WindowManager* m_WindowManager;
		};
	}
}