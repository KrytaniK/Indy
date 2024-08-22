module;

#include <cstdint>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Context;

import Indy.Graphics;

import :RenderPass;

export
{
	namespace Indy::Graphics
	{
		class VulkanContext : public RenderContext
		{
		public:
			VulkanContext(const VkInstance& instance, const uint32_t& id, const std::string& alias);
			VulkanContext(RenderContext* context, const VkInstance& instance);

			virtual ~VulkanContext() override;

			virtual const std::string& GetAlias() override { return m_Alias; };
			virtual const uint32_t& GetID() override { return m_ID; };

			virtual VulkanRenderPass& AddRenderPass(const std::string& alias) override;

			virtual VulkanRenderPass& AddRenderPass(const RenderPass* renderPass) override;

			virtual VulkanRenderPass& GetRenderPass(const uint32_t& id) override;

			virtual bool SetActiveViewport(const uint32_t& id) override;
			virtual bool SetActiveViewport(const std::string& alias) override;

		private:
			uint32_t m_ID;
			std::string m_Alias;
			VkInstance m_Instance;
			// Vulkan Device

			std::vector<Viewport> m_Viewports;
			std::vector<VulkanRenderPass> m_RenderPasses;
		};
	}
}