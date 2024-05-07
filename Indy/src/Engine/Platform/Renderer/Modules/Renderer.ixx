module;

#include <cstdint>
#include <memory>
#include <functional>
#include <vector>

export module Indy_Core_Renderer;

export
{
	namespace Indy
	{
		struct IWindowHandle;

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