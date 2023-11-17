#pragma once

#include "Engine/Platform/RendererAPI/RendererAPI.h"

namespace Engine
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void DrawFrame();

	private:
		static std::unique_ptr<RendererAPI> s_RendererAPI;
	};
}