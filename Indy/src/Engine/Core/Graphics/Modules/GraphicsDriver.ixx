module;

#include <cstdint>
#include <memory>
#include <string>

export module Indy.Graphics:Driver;

import :RenderContext;

export
{
	namespace Indy::Graphics
	{
		class Window;

		class Driver
		{
		public:
			enum Type : uint8_t { None = 0x00, Vulkan };

		public:
			Driver() = default;
			virtual ~Driver() = default;

			virtual Type GetType() = 0;

			virtual RenderContext* AddContext(const uint32_t& id, Window* window = nullptr) = 0;
			virtual RenderContext* AddContext(const std::string& key, Window* window = nullptr) =0;
			virtual RenderContext* AddContext(const uint32_t& id, const RenderContext& context, Window* window = nullptr) = 0;
			virtual RenderContext* AddContext(const std::string& key, const RenderContext& context, Window* window = nullptr) = 0;

			virtual bool RemoveContext(RenderContext* context) = 0;

			virtual RenderContext* GetContext(const uint32_t& key) = 0;
			virtual RenderContext* GetContext(const std::string& key) = 0;
		};

		std::unique_ptr<Driver> g_GraphicsDriver;
	}
}