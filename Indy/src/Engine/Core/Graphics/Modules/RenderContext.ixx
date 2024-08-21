module;

#include <cstdint>
#include <vector>
#include <string>
#include <memory>

export module Indy.Graphics:RenderContext;

import Indy.Window;

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
			virtual bool AddRenderPass(const RenderPass& renderPass) const = 0;

			// Starts the configuration chain for a new or existing render pass
			virtual const RenderContext& BeginRenderPass(const std::string& debugName) const = 0;
			// Starts the configuration chain for a new or existing render pass
			virtual const RenderContext& BeginRenderPass(const uint32_t& id) const = 0;

			// Ends the configuration chain for the current render pass
			virtual void EndRenderPass() const = 0;

			// Retrieves a render pass from the current render context via numerical id, if it exists.
			virtual const RenderPass& GetRenderPass(const uint32_t& id) const = 0;

			// ----- Shader Configurations -----

			// Adds a pre-processing shader to the render pass
			virtual const RenderContext& AddPreProcessShader(const std::string& shaderPath) const = 0;

			// Binds the vertex shader for the current render pass
			virtual const RenderContext& BindVertexShader(const std::string& shaderPath) const = 0;

			// Binds the tessellation control shader for the current render pass
			virtual const RenderContext& BindTessellationControlShader(const std::string& shaderPath) const = 0;

			// Binds the tessellation evaluation shader for the current render pass
			virtual const RenderContext& BindTessellationEvalShader(const std::string& shaderPath) const = 0;

			// Binds the geometry shader for the current render pass
			virtual const RenderContext& BindGeometryShader(const std::string& shaderPath) const = 0;

			// Binds the fragment shader for the current render pass
			virtual const RenderContext& BindFragmentShader(const std::string& shaderPath) const = 0;

			// Adds a post-processing shader to the render pass
			virtual const RenderContext& AddPostProcessShader(const std::string& shaderPath) const = 0;

			// ----- Rendering Configurations -----

			// Determines how vertices are constructed during graphics rendering for the current render pass
			virtual const RenderContext& SetTopology(const PrimitiveTopology& topology) const = 0;

			// Determines how polygons are rendered on screen for the current render pass
			virtual const RenderContext& SetPolygonMode(const PolygonMode& mode) const = 0;

			// Determines the direction that vertices will be constructed in, resulting in
			// either a front or rear facing face, for the current render pass
			virtual const RenderContext& SetFrontFace(const FrontFace& frontFace) const = 0;

			// Enables color blending for the current render pass
			virtual const RenderContext& SetBlendState(const bool enabled, const std::vector<ColorComponent>& masks = {}) const = 0;

			// Enables or disabled depth testing for the current render pass
			virtual const RenderContext& EnableDepthTesting(const bool enabled = true) const = 0;

			// Enables or disables depth writing for the current render pass
			virtual const RenderContext& EnableDepthWriting(const bool enabled = false) const = 0;

			// ----- Viewport Configurations -----

			virtual bool SetActiveViewport(const uint32_t& id) const = 0;
			virtual bool SetActiveViewport(const std::string& alias) const = 0;
		};
	}
}