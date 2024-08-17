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

	RenderContext* CreateRenderContext(const uint32_t& id, const std::string& debugName)
	{
		if (!g_GraphicsDriver)
		{
			INDY_CORE_WARN("Graphics Driver Is Not Initialized! Initialize A Driver With Graphics::Init()!!!");
			return nullptr;
		}

		return g_GraphicsDriver->CreateContext(id, debugName);
	}

	RenderContext* AddRenderContext(const RenderContext& context)
	{
		if (!g_GraphicsDriver)
		{
			INDY_CORE_WARN("Graphics Driver Is Not Initialized! Initialize A Driver With Graphics::Init()!!!");
			return nullptr;
		}

		return g_GraphicsDriver->AddContext(context);
	}

	bool RemoveContext(const uint32_t& id)
	{
		if (!g_GraphicsDriver)
		{
			INDY_CORE_WARN("Graphics Driver Is Not Initialized! Initialize A Driver With Graphics::Init()!!!");
			return false;
		}

		return g_GraphicsDriver->RemoveContext(id);
	}

	RenderContext* GetRenderContext(const uint32_t& key)
	{
		if (!g_GraphicsDriver)
		{
			INDY_CORE_WARN("Graphics Driver Is Not Initialized! Initialize A Driver With Graphics::Init()!!!");
			return nullptr;
		}

		return g_GraphicsDriver->GetContext(key);
	}

	bool SetActiveRenderContext(const uint32_t& id)
	{
		if (!g_GraphicsDriver)
		{
			INDY_CORE_WARN("Graphics Driver Is Not Initialized! Initialize A Driver With Graphics::Init()!!!");
			return false;
		}

		return g_GraphicsDriver->SetActiveContext(id);
	}

	bool SetActiveRenderContext(const RenderContext* context)
	{
		if (!g_GraphicsDriver)
		{
			INDY_CORE_WARN("Graphics Driver Is Not Initialized! Initialize A Driver With Graphics::Init()!!!");
			return false;
		}

		return g_GraphicsDriver->SetActiveContext(context);
	}

	bool Render(const Camera& camera)
	{
		if (!g_GraphicsDriver)
		{
			INDY_CORE_WARN("Graphics Driver Is Not Initialized! Initialize A Driver With Graphics::Init()!!!");
			return false;
		}

		return g_GraphicsDriver->Render(camera);
	}

}