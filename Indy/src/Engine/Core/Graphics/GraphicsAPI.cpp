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
		app.OnLoad.Subscribe<GraphicsAPI>(this, &GraphicsAPI::OnLoad);
		app.OnStart.Subscribe<GraphicsAPI>(this, &GraphicsAPI::OnStart);
		app.OnUnload.Subscribe<GraphicsAPI>(this, &GraphicsAPI::OnUnload);
	}
}
