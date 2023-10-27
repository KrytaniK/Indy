#pragma once

#include "Engine/Platform/RendererAPI/RendererAPI.h"

namespace Engine
{
	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		void onUpdate();

	private:
		std::unique_ptr<RendererAPI> m_RendererAPI;
	};
}