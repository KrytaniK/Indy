module;

#include <cstdint>
#include <memory>

export module Indy_Core_Renderer;

export
{
	namespace Indy
	{
		class IWindow;

		enum class RenderAPI : uint8_t { OpenGL = 0x00, Vulkan, DirectX, Metal };

		class IRenderer
		{
		public:
			virtual ~IRenderer() = default;

			virtual void Render() = 0;
			virtual RenderAPI GetType() = 0;
		};
	}
}