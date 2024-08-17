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
		struct RenderContext
		{
			// ----- Render Pass Operations -----

			// Copies an existing render pass to this render context
			virtual bool AddRenderPass(const RenderPass& renderPass) = 0;

			// Starts the configuration chain for a new or existing render pass
			virtual RenderContext* BeginRenderPass(const std::string& debugName) = 0;
			// Starts the configuration chain for a new or existing render pass
			virtual RenderContext* BeginRenderPass(const uint32_t& id) = 0;

			// Ends the configuration chain for the current render pass
			virtual void EndRenderPass() = 0;

			// Retrieves a render pass from the current render context via numerical id, if it exists.
			virtual RenderPass* GetRenderPass(const uint32_t& id) = 0;

			// ----- Shader Configurations -----

			// Adds a pre-processing shader to the render pass
			virtual RenderContext* AddPreProcessShader(const std::string& shaderPath) = 0;

			// Binds the vertex shader for the current render pass
			virtual RenderContext* BindVertexShader(const std::string& shaderPath) = 0;

			// Binds the tessellation control shader for the current render pass
			virtual RenderContext* BindTessellationControlShader(const std::string& shaderPath) = 0;

			// Binds the tessellation evaluation shader for the current render pass
			virtual RenderContext* BindTessellationEvalShader(const std::string& shaderPath) = 0;

			// Binds the geometry shader for the current render pass
			virtual RenderContext* BindGeometryShader(const std::string& shaderPath) = 0;

			// Binds the fragment shader for the current render pass
			virtual RenderContext* BindFragmentShader(const std::string& shaderPath) = 0;

			// Adds a post-processing shader to the render pass
			virtual RenderContext* AddPostProcessShader(const std::string& shaderPath) = 0;

			// ----- Rendering Configurations -----

			// Determines how vertices are constructed during graphics rendering for the current render pass
			virtual RenderContext* SetTopology(const PrimitiveTopology& topology) = 0;

			// Determines how polygons are rendered on screen for the current render pass
			virtual RenderContext* SetPolygonMode(const PolygonMode& mode) = 0;

			// Determines the direction that vertices will be constructed in, resulting in
			// either a front or rear facing face, for the current render pass
			virtual RenderContext* SetFrontFace(const FrontFace& frontFace) = 0;

			// Enables color blending for the current render pass
			virtual RenderContext* SetBlendState(const bool enabled, const std::vector<ColorComponent>& masks = {}) = 0;

			// Enables or disabled depth testing for the current render pass
			virtual RenderContext* EnableDepthTesting(const bool enabled = true) = 0;

			// Enables or disables depth writing for the current render pass
			virtual RenderContext* EnableDepthWriting(const bool enabled = false) = 0;
		};
	}
}