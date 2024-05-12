#include <Engine/Core/LogMacros.h>

import Indy.Layers;
import Indy.Window;

namespace Indy
{
	void ICL_WindowAction_Request::Execute(ILayerData* layerData)
	{
		ICL_WindowData_Request* actionData = static_cast<ICL_WindowData_Request*>(layerData);

		if (actionData->getActiveWindow)
		{
			actionData->handle = &m_WindowManager->GetActiveWindow();
			return;
		}

		actionData->handle = &m_WindowManager->GetWindow(actionData->index);
	}

	void ICL_WindowAction_Create::Execute(ILayerData* layerData)
	{
		ICL_WindowData_Create* actionData = static_cast<ICL_WindowData_Create*>(layerData);

		// Copy layer action data into window create info
		WindowCreateInfo createInfo;
		createInfo.title = actionData->title;
		createInfo.width = actionData->width;
		createInfo.height = actionData->height;
		createInfo.id = actionData->id;

		// Create window and attach the handle
		actionData->handle = &m_WindowManager->AddWindow(createInfo);
	}

	void ICL_WindowAction_Destroy::Execute(ILayerData* layerData)
	{
		ICL_WindowData_Destroy* actionData = static_cast<ICL_WindowData_Destroy*>(layerData);

		m_WindowManager->DestroyWindow(actionData->index);
	}

}