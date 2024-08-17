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
			Driver() = default;
			virtual ~Driver() = default;

			virtual Type GetType() = 0;

			// ---------- Render Context ----------

			virtual RenderContext* CreateContext(const uint32_t& id, const std::string& debugName) = 0;

			virtual RenderContext* AddContext(const RenderContext& context) = 0;

			virtual bool RemoveContext(const uint32_t& id) = 0;

			virtual RenderContext* GetContext(const uint32_t& key) = 0;

			virtual bool SetActiveContext(const uint32_t& id) = 0;
			virtual bool SetActiveContext(const RenderContext* context) = 0;

			// ---------- Data Submission ----------

			// virtual void DrawMesh(...MeshData...)

			// ---------- Rendering ----------

			virtual bool Render(const Camera& camera) = 0;
		};

		std::unique_ptr<Driver> g_GraphicsDriver;
	}
}