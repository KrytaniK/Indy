module;

#include <cstdint>
#include <vector>
#include <string>
#include <memory>

export module Indy.Graphics:Context;

import Indy.Window;

import :RenderPass;
import :Types;

export
{
	namespace Indy::Graphics
	{
		// An interface for defining, configuring, and modifying rendering contexts
		class Context
		{
		public:
			virtual ~Context() = default;

			virtual const std::string& GetAlias() = 0;
			virtual const uint32_t& GetID() = 0;

			// ----- Render Pass Operations -----

			// Creates a new render pass for this context.
			virtual RenderPass& AddRenderPass(const std::string& alias) = 0;

			// Retrieves a render pass from the current render context via numerical id, if it exists.
			virtual RenderPass& GetRenderPass(const uint32_t& id) = 0;

			// ----- Viewport Configurations -----

			virtual bool SetActiveViewport(const uint32_t& id) = 0;
			virtual bool SetActiveViewport(const std::string& alias) = 0;
		};
	}
}