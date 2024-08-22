#include <Engine/Core/LogMacros.h>

#include <memory>
#include <glm/glm.hpp>

#include <GLFW/glfw3.h>

import Indy.Graphics;
import Indy.VulkanGraphics;

import Indy.Window;
import Indy.Events;

namespace Indy::Graphics
{
	bool Init(const Driver::Type& driverType)
	{
		// Initialize the GLFW utility library
		if (glfwInit() == GLFW_FALSE)
			return false;

		// Set GLFW's error callback
		glfwSetErrorCallback([](int error, const char* description)
			{
				INDY_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
			}
		);

		// Initialize the graphics driver of choice
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
				// Tell GLFW not to create OpenGL contexts for created windows
				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

				// Create the graphics driver
				g_GraphicsDriver = std::make_unique<VulkanDriver>();

				return true;
			}
			default:
			{
				INDY_CORE_CRITICAL("Unsupported Graphics Driver!");
				return false;
			}
		}

		return true;
	}

	void Shutdown()
	{
		g_GraphicsDriver = nullptr;
	}

	RenderContext& CreateRenderContext(const std::string& alias)
	{
		if (!g_GraphicsDriver)
		{
			INDY_CORE_WARN("Graphics Driver Is Not Initialized! Initialize A Driver With Graphics::Init()!!!");
		}

		return g_GraphicsDriver->CreateContext(alias);
	}

	RenderContext& AddRenderContext(RenderContext* context, const std::string& alias)
	{
		if (!g_GraphicsDriver)
		{
			throw std::runtime_error("Graphics Driver Is Not Initialized! Initialize A Driver With Graphics::Init()!!!");
		}

		return g_GraphicsDriver->AddContext(context, alias);
	}

	bool RemoveContext(const uint32_t& id)
	{
		if (!g_GraphicsDriver)
		{
			throw std::runtime_error("Graphics Driver Is Not Initialized! Initialize A Driver With Graphics::Init()!!!");
			return false;
		}

		return g_GraphicsDriver->RemoveContext(id);
	}

	RenderContext& GetRenderContext(const uint32_t& key)
	{
		if (!g_GraphicsDriver)
		{
			throw std::runtime_error("Graphics Driver Is Not Initialized! Initialize A Driver With Graphics::Init()!!!");
		}

		return g_GraphicsDriver->GetContext(key);
	}

	RenderContext& GetRenderContext(const std::string& alias)
	{
		if (alias.empty())
			throw std::runtime_error("Invalid alias passed to Indy::Graphics::GetRenderContext(const std::string& alias)");

		return g_GraphicsDriver->GetContext(alias);
	}

	bool SetActiveRenderContext(const uint32_t& id, const uint32_t& defaultViewportID)
	{
		if (!g_GraphicsDriver)
		{
			throw std::runtime_error("Graphics Driver Is Not Initialized! Initialize A Driver With Graphics::Init()!!!");
			return false;
		}

		return g_GraphicsDriver->SetActiveContext(id) && g_GraphicsDriver->SetActiveViewport(defaultViewportID);
	}

	bool SetActiveRenderContext(const RenderContext& context, const uint32_t& defaultViewportID)
	{
		if (!g_GraphicsDriver)
		{
			throw std::runtime_error("Graphics Driver Is Not Initialized! Initialize A Driver With Graphics::Init()!!!");
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