module;

#include <cstdint>
#include <vector>
#include <string>
#include <memory>

export module Indy.Graphics:RenderContext;

import :Types;

export
{
	namespace Indy::Graphics
	{
		struct RenderContext
		{
			// Starts the configuration chain for a new render pass
			virtual RenderContext& AddRenderPass(const std::string& debugName) = 0;

			// Ends the configuration chain for the current render pass
			virtual void EndRenderPass() = 0;

			// Adds a pre-processing shader to the render pass  (Typically Compute operations)
			virtual RenderContext& AddPreProcess(const std::string& shaderPath) = 0;

			// Binds the vertex shader for this pass
			virtual RenderContext& BindVertexShader(const std::string& shaderPath) = 0;

			// Binds the tessellation control shader for this pass
			virtual RenderContext& BindTessellationControlShader(const std::string& shaderPath) = 0;

			// Binds the tessellation evaluation shader for this pass
			virtual RenderContext& BindTessellationEvalShader(const std::string& shaderPath) = 0;

			// Binds the geometry shader for this pass
			virtual RenderContext& BindGeometryShader(const std::string& shaderPath) = 0;

			// Binds the fragment shader for this pass
			virtual RenderContext& BindFragmentShader(const std::string& shaderPath) = 0;

			virtual RenderContext& SetTopology(const PrimitiveTopology& topology) = 0;

			virtual RenderContext& SetPolygonMode(const PolygonMode& mode) = 0;

			virtual RenderContext& SetFrontFace(const FrontFace& frontFace) = 0;

			virtual RenderContext& SetBlendState(const bool enabled, const std::vector<ColorComponent>& masks) = 0;

			virtual RenderContext& EnableDepthTesting(const bool enabled = true) = 0;

			virtual RenderContext& EnableDepthWriting(const bool enabled = false) = 0;
		};
	}
}