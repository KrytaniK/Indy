module;

#include <queue>
#include <vulkan/vulkan.h>

export module Indy.VulkanRenderer:RenderTarget;

import Indy.Renderer;

export
{
	namespace Indy
	{
		class VulkanRenderTarget;

		struct VulkanContext : IRenderContext
		{
			VkInstance* instance = nullptr;
			VulkanRenderTarget* renderTarget = nullptr;
		};

		class VulkanRenderTarget : public IRenderTarget
		{
		public:
			virtual void Initialize() override;

			virtual void PrepareRenderPass() override;
			virtual void BeginRenderPass() override;
			virtual void EndRenderPass() override;

			virtual VulkanContext* GetHandle() override;
			virtual uint32_t GetWidth() override;
			virtual uint32_t GetHeight() override;

			virtual bool Prepared() override;

		private:
			VulkanContext m_Handle;
			bool m_Prepared = false;
		};
	}
}