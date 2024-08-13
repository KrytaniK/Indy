module;

#include <cstdint>
#include <string>

export module Indy.Graphics:Commands;

import :Driver;
import :RenderContext;

export
{
	namespace Indy::Graphics
	{
		class Window;

		bool Init(const Driver::Type& driverType);

		RenderContext* AddRenderContext(const uint32_t& id, Window* window = nullptr);
		RenderContext* AddRenderContext(const std::string& key, Window* window = nullptr);
		RenderContext* AddRenderContext(const uint32_t& id, const RenderContext& context, Window* window = nullptr);
		RenderContext* AddRenderContext(const std::string& key, const RenderContext& context, Window* window = nullptr);

		bool RemoveContext(RenderContext* context);

		RenderContext* GetContext(const uint32_t& key);
		RenderContext* GetContext(const std::string& key);
	}
}