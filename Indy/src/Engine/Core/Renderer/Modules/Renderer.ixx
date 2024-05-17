module;

#include <cstdint>
#include <memory>

export module Indy.Renderer:IRenderer;

import Indy.Events;

export
{
	namespace Indy
	{
		// Utility Enum for defining supported Rendering APIs.
		enum class RenderAPI : uint8_t { None = 0x00, OpenGL, Vulkan, DirectX, Metal };

		// Tag structure for platform-specific render contexts
		struct IRenderContext {};

		// Tag structure for platform-specific render contexts
		struct IRenderCommand {};

		struct IRenderCommandEvent : IEvent
		{
			uint32_t rendererID;
		};

		// Interface for platform-specific renderers.
		class IRenderer
		{
		public:
			virtual ~IRenderer() = default;

			virtual void Render() = 0;

			virtual uint32_t GetRendererID() = 0;

		private:
			virtual void OnCommand(IRenderCommandEvent* event) = 0;
		};

		std::unique_ptr<IRenderer> CreateRenderer(const RenderAPI& api);
	}
}
