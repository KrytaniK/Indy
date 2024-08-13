#include <Engine/Core/LogMacros.h>

#include <glm/glm.hpp>

import Indy.Graphics;

import Indy.Window;
import Indy.Events;

namespace Indy::Graphics
{
	bool Init(const Driver::Type& driverType)
	{
		switch (driverType)
		{
			case Driver::Type::None:
			{
				INDY_CORE_ERROR("Invalid Graphics Driver Type. Please choose from one of the following:");
				INDY_CORE_INFO("Indy::Graphics::Driver::Vulkan");
				return false;
			}
			case Driver::Type::Vulkan:
			{
				INDY_CORE_WARN("The Vulkan Driver Has Not Been Implemented!");
				return false;
			}
			default:
			{
				INDY_CORE_CRITICAL("Unsupported Graphics Driver!");
				return false;
			}
		}

		return true;
	}

	RenderContext* AddRenderContext(const uint32_t& id, Window* window)
	{
		if (!g_GraphicsDriver)
		{
			INDY_CORE_WARN("Graphics Driver Is Not Initialized! Initialize A Driver With Graphics::Init()!!!");
			return nullptr;
		}

		return g_GraphicsDriver->AddContext(id, window);
	}

	RenderContext* AddRenderContext(const std::string& key, Window* window)
	{
		if (!g_GraphicsDriver)
		{
			INDY_CORE_WARN("Graphics Driver Is Not Initialized! Initialize A Driver With Graphics::Init()!!!");
			return nullptr;
		}

		return g_GraphicsDriver->AddContext(key, window);
	}

	RenderContext* AddRenderContext(const uint32_t& id, const RenderContext& context, Window* window)
	{
		if (!g_GraphicsDriver)
		{
			INDY_CORE_WARN("Graphics Driver Is Not Initialized! Initialize A Driver With Graphics::Init()!!!");
			return nullptr;
		}

		return g_GraphicsDriver->AddContext(id, context, window);
	}

	RenderContext* AddRenderContext(const std::string& key, const RenderContext& context, Window* window)
	{
		if (!g_GraphicsDriver)
		{
			INDY_CORE_WARN("Graphics Driver Is Not Initialized! Initialize A Driver With Graphics::Init()!!!");
			return nullptr;
		}

		return g_GraphicsDriver->AddContext(key, context, window);
	}

	bool RemoveContext(RenderContext* context)
	{
		return g_GraphicsDriver->RemoveContext(context);
	}

	RenderContext* GetContext(const uint32_t& key)
	{
		return g_GraphicsDriver->GetContext(key);
	}

	RenderContext* GetContext(const std::string& key)
	{
		return g_GraphicsDriver->GetContext(key);
	}

}