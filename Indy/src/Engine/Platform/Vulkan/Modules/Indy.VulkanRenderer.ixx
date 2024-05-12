module;

#include <cstdint>

export module Indy.VulkanRenderer;

import Indy.Renderer;

export
{
	namespace Indy
	{
		class VulkanRenderer : public IRenderer
		{
		public:
			VulkanRenderer();
			VulkanRenderer(IWindowHandle* windowHandle);
			~VulkanRenderer();

			virtual void Render() override;
			virtual void Init() override;

		private:
			void DrawFrame();

		private:
			uint8_t m_CurrentFrame;
		};
	}
}