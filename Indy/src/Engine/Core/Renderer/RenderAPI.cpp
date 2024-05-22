#include "Engine/Core/LogMacros.h"
#include <memory>


import Indy.Application;
import Indy.Graphics;
import Indy.VulkanGraphics;

namespace Indy::Graphics
{
	std::unique_ptr<IRenderAPI> CreateRenderAPI(const RenderAPI& api)
	{
		switch (api)
		{
			case RenderAPI::Vulkan: return std::make_unique<VulkanAPI>();
			default:
				return nullptr;
		}
	}

	IRenderAPI::IRenderAPI()
	{
		INDY_CORE_WARN("Base Render API Constructor");
		Application& app = Application::Get();
		app.Load.Subscribe<IRenderAPI>(this, &IRenderAPI::OnLoad);
		app.OnStart_Event.Subscribe<IRenderAPI>(this, &IRenderAPI::OnStart);
		app.Unload.Subscribe<IRenderAPI>(this, &IRenderAPI::OnUnload);
	}
}
