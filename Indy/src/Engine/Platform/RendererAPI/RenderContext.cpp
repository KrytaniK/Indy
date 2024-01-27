#include "RenderContext.h"

#include "Engine/Core/LogMacros.h"
#include "Engine/Platform/RendererAPI/Vulkan/Context.h"

namespace Engine
{
	RenderContext::Type RenderContext::s_Type;

	// Creates an instance of the Rendering API set by the application. Defaults to Vulkan
	std::unique_ptr<RenderContext> RenderContext::Create()
	{
		switch (s_Type)
		{
		case RenderContext::Type::OpenGL:
			INDY_CORE_CRITICAL("OpenGL Currently Not Supported!");
			break;
		case RenderContext::Type::Vulkan:
			return std::make_unique<VulkanAPI::Context>();
		default:
			return std::make_unique<VulkanAPI::Context>();
		}

		INDY_CORE_CRITICAL("Render API Not Recognized!");

		return nullptr;
	}
}