module;

#include <cstdint>

export module Indy.Renderer:IRenderTarget;

export
{
	namespace Indy
	{
		struct IRenderContext;

		// Base interface for declaring Render Targets
		class IRenderTarget
		{
		public:
			virtual ~IRenderTarget() {};

			virtual void Initialize() = 0;

			virtual void PrepareRenderPass() = 0;
			virtual void BeginRenderPass() = 0;
			virtual void EndRenderPass() = 0;

			virtual IRenderContext* GetHandle() = 0;
			virtual uint32_t GetWidth() = 0;
			virtual uint32_t GetHeight() = 0;
			virtual bool Prepared() = 0;
		};
	}
}
