module;

export module Indy.Graphics:IRenderTarget;

export
{
	namespace Indy::Graphics
	{
		// Base interface for declaring Render Targets
		class IRenderTarget
		{
		public:
			IRenderTarget() = default;
			virtual ~IRenderTarget() = default;
		};
	}
}
