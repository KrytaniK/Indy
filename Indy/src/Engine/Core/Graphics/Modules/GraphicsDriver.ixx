module;

#include <cstdint>
#include <memory>
#include <string>

export module Indy.Graphics:Driver;

import :RenderContext;
import :Camera;
import :Types;

export
{
	namespace Indy::Graphics
	{
		class Driver
		{
		public:
			enum Type : uint8_t { None = 0x00, Vulkan };

		public:
			virtual ~Driver() = default;

			virtual Type GetType() = 0;

			// ---------- Render Context ----------

			virtual const RenderContext& CreateContext(const std::string& alias) = 0;

			virtual const RenderContext& AddContext(const RenderContext& context) = 0;

			virtual bool RemoveContext(const uint32_t& id) = 0;

			virtual const RenderContext& GetContext(const uint32_t& id) = 0;
			virtual const RenderContext& GetContext(const std::string& alias) = 0;

			virtual bool SetActiveContext(const uint32_t& id) = 0;
			virtual bool SetActiveContext(const RenderContext& context) = 0;

			// ---------- Data Submission ----------

			

			// ---------- Rendering ----------

			virtual bool SetActiveViewport(const uint32_t& id) = 0;
			virtual bool SetActiveViewport(const std::string& alias) = 0;

			virtual bool Render(const Camera& camera) = 0;
		};

		std::unique_ptr<Driver> g_GraphicsDriver = nullptr;
	}
}