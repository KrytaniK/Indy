module;

#include <cstdint>
#include <vector>
#include <string>
#include <memory>

export module Indy.Graphics:RenderContext;

import Indy.Window;

import :RenderPass;
import :Types;

export
{
	namespace Indy::Graphics
	{
		// An interface for defining, configuring, and modifying rendering contexts
		class RenderContext
		{
		public:
			virtual ~RenderContext() = default;

			virtual const std::string& GetAlias() const = 0;
			virtual uint32_t GetID() const = 0;

			// ----- Render Pass Operations -----

			// Copies an existing render pass to this render context
			virtual bool AddRenderPass(const RenderPass* renderPass)= 0;

			// Retrieves a render pass from the current render context via numerical id, if it exists.
			virtual const RenderPass& GetRenderPass(const uint32_t& id) = 0;

			// ----- Viewport Configurations -----

			virtual bool SetActiveViewport(const uint32_t& id) const = 0;
			virtual bool SetActiveViewport(const std::string& alias) const = 0;
		};
	}
}