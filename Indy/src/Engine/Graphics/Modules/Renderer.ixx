module;

export module Indy.Graphics:Renderer;

export
{
	namespace Indy
	{
		// Interface for API-specific renderers.
		class Renderer
		{
		public:
			virtual ~Renderer() = default;

			virtual void Render() = 0;

			virtual void Enable() = 0;
			virtual void Disable() = 0;
		};
	}
}
