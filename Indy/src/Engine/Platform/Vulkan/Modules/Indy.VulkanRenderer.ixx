module;

#include <memory>
#include <vector>
#include <queue>

export module Indy.VulkanRenderer;

export import :Backend;
export import :Device;
export import :Queue;
export import :RenderTarget;
export import :Commands;
export import :CommandArgs;
export import :Utils;

import Indy.Renderer;

export
{
	namespace Indy
	{
		class VulkanRenderer : public IRenderer
		{
		public:
			VulkanRenderer();
			virtual ~VulkanRenderer() override;

			virtual void Render() override;

			virtual uint32_t GetRendererID() override;

		private:
			virtual void OnCommand(IRenderCommandEvent* event) override;

		private:
			uint32_t m_RendererID;
			std::unique_ptr<VulkanBackend> m_Vulkan_Backend;
			std::vector<VulkanRenderTarget> m_RenderTargets;
			std::vector<std::unique_ptr<IVulkanCommand>> m_Commands;
			std::queue<VkCommandEvent> m_CommandQueue;
		};
	}
}