#include <Engine/Core/LogMacros.h>

#include <glm/glm.hpp>

import Indy.Graphics;
//import Indy.VulkanGraphics;

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
				/*
				g_GraphicsDriver = std::make_unique<VulkanDriver>();
				return true;
				*/
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

	RenderContext* CreateRenderContext(const uint32_t& id, const std::string& alias)
	{
		if (!g_GraphicsDriver)
		{
			INDY_CORE_WARN("Graphics Driver Is Not Initialized! Initialize A Driver With Graphics::Init()!!!");
			return nullptr;
		}

		return g_GraphicsDriver->CreateContext(id, alias);
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

	RenderContext* GetRenderContext(const std::string& alias)
	{
		if (alias.empty())
			return nullptr;

		return g_GraphicsDriver->GetContext(alias);
	}

	bool SetActiveRenderContext(const uint32_t& id, const uint32_t& defaultViewportID)
	{
		if (!g_GraphicsDriver)
		{
			INDY_CORE_WARN("Graphics Driver Is Not Initialized! Initialize A Driver With Graphics::Init()!!!");
			return false;
		}

		return g_GraphicsDriver->SetActiveContext(id) && g_GraphicsDriver->SetActiveViewport(defaultViewportID);
	}

	bool SetActiveRenderContext(const RenderContext* context, const uint32_t& defaultViewportID)
	{
		if (!context)
		{
			INDY_CORE_ERROR("Failed to set the active render context: Invalid context pointer.");
			return false;
		}

		if (!g_GraphicsDriver)
		{
			INDY_CORE_WARN("Graphics Driver Is Not Initialized! Initialize A Driver With Graphics::Init()!!!");
			return false;
		}

		// Set driver active context and default viewport
		return g_GraphicsDriver->SetActiveContext(context) && g_GraphicsDriver->SetActiveViewport(defaultViewportID);
	}

	bool SetActiveViewport(const uint32_t& id)
	{
		return g_GraphicsDriver->SetActiveViewport(id);
	}

	bool SetActiveViewport(const std::string& alias)
	{
		return g_GraphicsDriver->SetActiveViewport(alias);
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