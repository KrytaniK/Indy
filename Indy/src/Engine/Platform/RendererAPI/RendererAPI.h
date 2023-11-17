#pragma once

#include "Engine/EventSystem/Events.h"
#include <memory>

namespace Engine
{
	class RendererAPI
	{
	public:
		enum class Type
		{
			None = 0, OpenGL, Vulkan,
		};

	private:
		static Type s_Type; // Must be set manually by the user

	public:
		static Type Get() { return s_Type; };
		static void Set(Type type)
		{
			if (s_Type != Type::None) // Type shouldn't change at runtime.
				return;

			s_Type = type;
		}

	public:
		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		virtual void DrawFrame() = 0;

		virtual void onWindowResize(Event& event) = 0;

		// Creates an instance of the Rendering API set by the application. Defaults to Vulkan
		static std::unique_ptr<RendererAPI> Create();
	};
}

#define RENDERER_API_VULKAN Engine::RendererAPI::Type::Vulkan