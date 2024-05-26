module;

export module Indy.VulkanGraphics;

export import :Backend;
export import :Constants;
export import :Utils;
export import :Device;
export import :Queue;
export import :Events;
export import :RenderTarget;
export import :Swapchain;

import Indy.Graphics;

export
{
	namespace Indy::Graphics
	{
		class VulkanRenderer : public IRenderer
		{
		public:
			VulkanRenderer();
			virtual ~VulkanRenderer() override;

			virtual void Render() override;
		};
	}
}