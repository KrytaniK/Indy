#include "RendererAPI.h"

#include "Engine/Core/Log.h"
#include "Engine/Platform/RendererAPI/Vulkan/VulkanAPI.h"

namespace Engine
{
	RendererAPI::Type RendererAPI::s_Type;

	// Creates an instance of the Rendering API set by the application. Defaults to Vulkan
	std::unique_ptr<RendererAPI> RendererAPI::Create()
	{
		switch (s_Type)
		{
		case RendererAPI::Type::OpenGL:
			INDY_CORE_CRITICAL("OpenGL Currently Not Supported!");
			break;
		case RendererAPI::Type::Vulkan:
			return std::make_unique<VulkanAPI>();
		default:
			return std::make_unique<VulkanAPI>();
		}

		INDY_CORE_CRITICAL("Render API Not Recognized!");

		return nullptr;
	}
}