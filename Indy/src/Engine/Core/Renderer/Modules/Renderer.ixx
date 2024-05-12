module;

#include <cstdint>

export module Indy.Renderer:IRenderer;

import Indy.Window;

export
{
	namespace Indy
	{
		enum class RenderAPI : uint8_t { OpenGL = 0x00, Vulkan, DirectX, Metal };

		class IRenderer
		{
		public:
			virtual ~IRenderer() = default;

			virtual void Render() = 0;
			virtual void Init() = 0;

		protected:
			IWindowHandle* m_WindowHandle;
		};
	}
}
