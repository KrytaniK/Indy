module;

#include <cstdint>
#include <memory>
#include <string>

export module Indy.Graphics:Commands;

import :Driver;
import :Context;
import :Camera;
import :Types;

export
{
	namespace Indy::Graphics
	{
		// ---------- Driver Initialization and Shutdown ----------

		bool Init(const Driver::Type& driverType);

		void Shutdown();

		// ---------- Render Context ----------

		Context& CreateContext(const std::string& alias);

		bool RemoveContext(const uint32_t& id);

		Context& GetContext(const uint32_t& id);
		Context& GetContext(const std::string& alias);

		bool SetActiveContext(const uint32_t& id, const uint32_t& defaultViewportID = 0);

		// ---------- Data Submission ----------

		

		// ---------- Rendering ----------

		bool SetActiveViewport(const uint32_t& id);
		bool SetActiveViewport(const std::string& alias);

		bool Render(const Camera& camera);
	}
}