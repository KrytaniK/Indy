#pragma once

#include "Engine/Platform/Rendering/RenderContext.h"
#include "RenderUtils.h"

#include <vector>

namespace Engine
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginFrame();
		static void EndFrame(Camera& camera);
		static void DrawFrame();

		static void Draw(Vertex* vertices, uint32_t vertexCount);
		static void DrawIndexed(Vertex* vertices, uint32_t vertexCount, uint32_t* indices, uint32_t indexCount);
		static void DrawInstanced(Vertex* vertices, uint32_t vertexCount, uint32_t* indices, uint32_t indexCount, uint32_t instanceCount);

	private:
		static std::unique_ptr<RenderContext> s_RenderContext;
	};
}