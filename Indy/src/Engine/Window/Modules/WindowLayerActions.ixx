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
		
		// Base Window Layer Data
		struct WindowActionData : ILayerData
		{
			std::weak_ptr<WindowManager> windowManager;
		};

		// Layer Data for creating windows
		struct AD_WindowCreateInfo : WindowActionData
		{
			std::string title = "Indy Engine";
			unsigned int width = 1280, height = 760;
			uint8_t id = 0;
			IWindowHandle* windowHandle = nullptr;
		};

		// Layer Data for destroying windows
		struct AD_WindowDestroyInfo : WindowActionData
		{
			uint8_t index; // Index of the target window
		};

		struct AD_WindowRequestInfo : WindowActionData
		{
			uint8_t index; // Index of the target window. Used if boolean getActiveWindow is set to false.
			bool getActiveWindow; // If set to true, requests the window that is currently in focus.
			IWindowHandle* windowHandle; // Pointer to a base window handle for the requested window.
		};

		// ----- Layer Actions -----

		// Layer Action for creating windows
		class LA_WindowCreate : public ILayerAction
		{
		public:
			virtual void Execute(ILayerData* layerData) override;
		};


		// Layer Action for destroying windows
		class LA_WindowDestroy : public ILayerAction
		{
		public:
			virtual void Execute(ILayerData* layerData) override;
		};

		// Layer Action for requesting a window
		class LA_WindowRequest : public ILayerAction
		{
		public:
			virtual void Execute(ILayerData* layerData) override;
		};
	}
}