#pragma once

#include "Engine/Platform/RendererAPI/RenderContext.h"

namespace Engine
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginFrame();
		static void EndFrame();
		static void DrawFrame();

	private:
		static std::unique_ptr<RenderContext> s_RenderContext;

		// Temp
		static void OnRender();
	};
}