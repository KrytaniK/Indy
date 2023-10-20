#pragma once

#include "../RendererAPI.h"

#include <vulkan/vulkan.h>

namespace Engine
{
	class VulkanAPI : public RendererAPI
	{
	public:
		VulkanAPI();
		~VulkanAPI();

	public:
		virtual void Init() override;
		virtual void Shutdown() override;

	private:
		void onApplicationClose(Event& event);

	private:
		static VkInstance s_Vulkan_Instance;
	};
}