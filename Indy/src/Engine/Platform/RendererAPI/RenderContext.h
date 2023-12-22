#pragma once

#include "Engine/EventSystem/Events.h"
#include "Engine/Renderer/Camera.h"
#include <memory>

namespace Engine
{
	class RenderContext
	{
	public:
		enum class Type
		{
			None = 0, OpenGL, Vulkan,
		};

		enum class DataSubmitType
		{
			Vertex = 0, Index,
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
		virtual void Init(void* window) = 0;
		virtual void Shutdown() = 0;
		virtual void CreateContext(void* window) = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame(Camera& camera) = 0;
		virtual void DrawFrame() = 0;
		virtual void SwapBuffers() = 0;

		virtual void Submit(void* vertices, uint32_t vertexCount, void* indices, uint32_t indexCount, uint32_t instanceCount) = 0;

		virtual void onWindowResize(Event& event) = 0;

		// Creates an instance of the Rendering API set by the application. Defaults to Vulkan
		static std::unique_ptr<RenderContext> Create();
	};
}

#define RENDER_API_VULKAN Engine::RenderContext::Type::Vulkan