module;

#include <cstdint>
#include <string>

export module Indy.Graphics:Commands;

import :Driver;
import :RenderContext;
import :Camera;
import :Types;

export
{
	namespace Indy::Graphics
	{
		// ---------- Driver Initialization ----------

		bool Init(const Driver::Type& driverType);

		// ---------- Render Context ----------

		RenderContext* CreateRenderContext(const uint32_t& id, const std::string& debugName);

		RenderContext* AddRenderContext(const RenderContext& context);

		bool RemoveContext(const uint32_t& id);

		RenderContext* GetRenderContext(const uint32_t& key);

		bool SetActiveRenderContext(const uint32_t& id);
		bool SetActiveRenderContext(const RenderContext* context);

		// ---------- Data Submission ----------

		// void DrawMesh(...MeshData...)

		// ---------- Rendering ----------

		bool Render(const Camera& camera);
	}
}