module;

#include <cstdint>

export module Indy.Graphics:RenderTarget;

export
{
	namespace Indy
	{
		// Base interface for declaring Render Targets
		class RenderTarget
		{
		public:
			RenderTarget() = default;
			virtual ~RenderTarget() = default;

			virtual const uint32_t& GetID() const = 0;
		};
	}
}
