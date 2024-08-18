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

		RenderContext* CreateRenderContext(const uint32_t& id, const std::string& alias);

		RenderContext* AddRenderContext(const RenderContext& context);

		bool RemoveContext(const uint32_t& id);

		RenderContext* GetRenderContext(const uint32_t& id);
		RenderContext* GetRenderContext(const std::string& alias);

		bool SetActiveRenderContext(const uint32_t& id, const uint32_t& defaultViewportID = 0);
		bool SetActiveRenderContext(const RenderContext* context, const uint32_t& defaultViewportID = 0);

		// ---------- Data Submission ----------

		

		// ---------- Rendering ----------

		bool SetActiveViewport(const uint32_t& id);
		bool SetActiveViewport(const std::string& alias);

		bool Render(const Camera& camera);
	}
}