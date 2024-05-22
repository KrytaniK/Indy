module;

export module Indy.Graphics:IRenderer;

import Indy.Events;

export
{
	namespace Indy::Graphics
	{
		// Interface for platform-specific renderers.
		class IRenderer
		{
		public:
			virtual ~IRenderer() = default;

			virtual void Render() = 0;
		};
	}
}
