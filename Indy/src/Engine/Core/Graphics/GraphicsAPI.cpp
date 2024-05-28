#include <Engine/Core/LogMacros.h>

#include <memory>

import Indy.Application;
import Indy.Graphics;
import Indy.VulkanGraphics;

namespace Indy
{
	std::unique_ptr<GraphicsAPI> GraphicsAPI::Create(const GraphicsAPI::Type& api)
	{
		switch (api)
		{
			case Vulkan: return std::make_unique<VulkanAPI>();
			default:
			{
				INDY_CORE_CRITICAL("Unsupported Graphics API!");
				return nullptr;
			}
		}
	}

	GraphicsAPI::GraphicsAPI()
	{
		Application& app = Application::Get();
		app.Load.Subscribe<GraphicsAPI>(this, &GraphicsAPI::OnLoad);
		app.OnStart_Event.Subscribe<GraphicsAPI>(this, &GraphicsAPI::OnStart);
		app.Unload.Subscribe<GraphicsAPI>(this, &GraphicsAPI::OnUnload);
	}
}
