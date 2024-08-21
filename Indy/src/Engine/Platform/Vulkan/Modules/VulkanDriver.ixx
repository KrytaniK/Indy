module;

#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include <map>

#include <vulkan/vulkan.h>

export module Indy.VulkanGraphics:Driver;

import Indy.Graphics;

import :Context;
//import :Device;

export
{
	namespace Indy::Graphics
	{
		class VulkanDriver : public Driver
		{
		public:
			VulkanDriver();
			virtual ~VulkanDriver() override;

			virtual Driver::Type GetType() override;

			// ---------- Render Context ----------

			virtual const RenderContext& CreateContext(const std::string& alias) override;

			virtual const RenderContext& AddContext(RenderContext* context, const std::string& alias) override;

			virtual bool RemoveContext(const uint32_t& id) override;

			virtual const RenderContext& GetContext(const uint32_t& id) override;
			virtual const RenderContext& GetContext(const std::string& alias) override;

			virtual bool SetActiveContext(const uint32_t& id) override;
			virtual bool SetActiveContext(const RenderContext& context) override;

			// ---------- Data Submission ----------



			// ---------- Rendering ----------

			virtual bool SetActiveViewport(const uint32_t& id) override;
			virtual bool SetActiveViewport(const std::string& alias) override;

			virtual bool Render(const Camera& camera) override;

		private:
			bool Initialize();
			bool QueryValidationLayerSupport(const std::vector<const char*>& layers);
			std::vector<const char*> GetGLFWExtensions();
			bool CreateDebugMessenger(const VkDebugUtilsMessengerCreateInfoEXT& info);
			void ConfigureVulkanLogicalDevices();

		private:
			VkInstance m_Instance;
			VkDebugUtilsMessengerEXT m_DebugMessenger;
			// VulkanDeviceConfig m_DeviceConfig;

			// std::unique_ptr<VulkanRenderer> m_Renderer;

			std::map<uint32_t, std::unique_ptr<VulkanContext>> m_Contexts;
			uint32_t m_ActiveContext;
			uint32_t m_ActiveViewport;
		};
	}
}