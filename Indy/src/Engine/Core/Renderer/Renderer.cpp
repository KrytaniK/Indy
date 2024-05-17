
#include <memory>

#include "Engine/Core/LogMacros.h"

import Indy.Renderer;
import Indy.VulkanRenderer;

namespace Indy
{
	std::unique_ptr<IRenderer> CreateRenderer(const RenderAPI& api)
	{
		switch (api)
		{
			case RenderAPI::None:		return nullptr;
			case RenderAPI::OpenGL:		return nullptr;
			case RenderAPI::Vulkan:		return std::make_unique<VulkanRenderer>();
			case RenderAPI::DirectX:	return nullptr;
			case RenderAPI::Metal:		return nullptr;
			default:					return nullptr;
		}
	}
}