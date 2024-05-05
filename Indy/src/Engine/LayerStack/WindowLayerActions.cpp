#include "Engine/Core/LogMacros.h"
#include <memory>

import Indy_Core_LayerStack;
import Indy_Core_WindowLayer;

namespace Indy
{
	void LA_WindowRequest::Execute(ILayerData* layerData)
	{
		AD_WindowRequestInfo* windowRequestInfo = static_cast<AD_WindowRequestInfo*>(layerData);

		if (windowRequestInfo->windowManager.expired())
		{
			INDY_CORE_ERROR(
				"Could not create window [{0}]. Bad Window Manager", 
				windowRequestInfo->index
			);
			return;
		}

		std::shared_ptr<WindowManager> windowManager = windowRequestInfo->windowManager.lock();

		if (windowRequestInfo->getActiveWindow)
		{
			windowRequestInfo->windowHandle = &windowManager->GetActiveWindow();
			return;
		}

		windowRequestInfo->windowHandle = &windowManager->GetWindow(windowRequestInfo->index);
	}

	void LA_WindowCreate::Execute(ILayerData* layerData)
	{
		AD_WindowCreateInfo* windowCreateInfo = static_cast<AD_WindowCreateInfo*>(layerData);

		if (windowCreateInfo->windowManager.expired())
		{
			INDY_CORE_ERROR(
				"Could not create window [{0}]. Bad Window Manager",
				windowCreateInfo->title
			);
			return;
		}

		std::shared_ptr<WindowManager> windowManager = windowCreateInfo->windowManager.lock();

		// Copy layer action data into window create info
		WindowCreateInfo createInfo;
		createInfo.title = windowCreateInfo->title;
		createInfo.width = windowCreateInfo->width;
		createInfo.height = windowCreateInfo->height;
		createInfo.id = windowCreateInfo->id;

		// Create window and attach the handle
		windowCreateInfo->windowHandle = &windowManager->AddWindow(createInfo);
	}

	void LA_WindowDestroy::Execute(ILayerData* layerData)
	{
		AD_WindowDestroyInfo* windowDestroyInfo = static_cast<AD_WindowDestroyInfo*>(layerData);

		if (windowDestroyInfo->windowManager.expired())
		{
			INDY_CORE_ERROR(
				"Could not create window [{0}]. Bad Window Manager",
				windowDestroyInfo->index
			);
			return;
		}

		std::shared_ptr<WindowManager> windowManager = windowDestroyInfo->windowManager.lock();

		windowManager->DestroyWindow(windowDestroyInfo->index);
	}

}