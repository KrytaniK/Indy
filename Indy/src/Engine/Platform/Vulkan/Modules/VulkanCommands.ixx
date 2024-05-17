module;

#include <cstdint>
#include <vulkan/vulkan.h>

export module Indy.VulkanRenderer:Commands;

import :CommandArgs;

import Indy.Renderer;

export
{
	namespace Indy
	{
		struct VulkanContext;

		// Enum class for describing vulkan render commands
		enum class VkCommandType : uint8_t { Clear = 0x00, Draw, DrawIndexed, DrawIndirect, DrawInstanced };

		// Event structure for calling vulkan commands
		struct VkCommandEvent : IRenderCommandEvent
		{
			VulkanContext* context;
			VkCommandType command;
			VkCommandArgs args;
		};

		// Base Vulkan Command
		class IVulkanCommand
		{
		public:
			virtual ~IVulkanCommand() = default;

			virtual void Execute(const VulkanContext& context, VkCommandArgs* args) = 0;
		};

		// ---------------------------------------------------------
		// Command Declarations

		class VkCommand_Clear final : public IVulkanCommand
		{
		public:
			VkCommand_Clear();
			virtual void Execute(const VulkanContext& context, VkCommandArgs* args) override;
		};

		class VkCommand_Draw final : public IVulkanCommand
		{
		public:
			VkCommand_Draw();
			virtual void Execute(const VulkanContext& context, VkCommandArgs* args) override;
		};

		class VkCommand_DrawIndexed final : public IVulkanCommand
		{
		public:
			VkCommand_DrawIndexed();
			virtual void Execute(const VulkanContext& context, VkCommandArgs* args) override;
		};

		class VkCommand_DrawIndirect final : public IVulkanCommand
		{
		public:
			VkCommand_DrawIndirect();
			virtual void Execute(const VulkanContext& context, VkCommandArgs* args) override;
		};

		class VkCommand_DrawInstanced final : public IVulkanCommand
		{
		public:
			VkCommand_DrawInstanced();
			virtual void Execute(const VulkanContext& context, VkCommandArgs* args) override;
		};
	}
}