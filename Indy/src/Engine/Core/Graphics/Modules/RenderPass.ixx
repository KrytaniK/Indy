module;

#include <cstdint>
#include <string>
#include <vector>

export module Indy.Graphics:RenderPass;

import :Types;

export
{
	namespace Indy::Graphics
	{
		class RenderPass
		{
		public:
			virtual ~RenderPass() = default;

			virtual const std::string& GetAlias() = 0;
			virtual const uint32_t& GetID() = 0;

			// Begins the configuration chain for this render pass
			virtual RenderPass& Begin() = 0;

			// Ends the configuration chain for this render pass
			virtual void End() = 0;

			// ---------- Pipeline Shader Configuration ----------

			// Binds a Pre-Process shader (usually a compute shader) to this render pass
			virtual RenderPass& AddPreProcessShader(const std::string& shaderPath) = 0;

			// Binds a single vertex shader to this render pass. If a shader is already bound,
			// it will be replaced with the new one.
			virtual RenderPass& BindVertexShader(const std::string& shaderPath) = 0;

			// Binds a single tessellation control shader to this render pass. If a shader is already bound,
			// it will be replaced with the new one.
			virtual RenderPass& BindTessellationControlShader(const std::string& shaderPath) = 0;

			// Binds a single tessellation evaluation shader to this render pass. If a shader is already bound,
			// it will be replaced with the new one.
			virtual RenderPass& BindTessellationEvalShader(const std::string& shaderPath) = 0;

			// Binds a single geometry shader to this render pass. If a shader is already bound,
			// it will be replaced with the new one.
			virtual RenderPass& BindGeometryShader(const std::string& shaderPath) = 0;

			// Binds a single fragment shader to this render pass. If a shader is already bound,
			// it will be replaced with the new one.
			virtual RenderPass& BindFragmentShader(const std::string& shaderPath) = 0;

			// Adds a post process shader (fragment or compute) to this render pass.
			virtual RenderPass& AddPostProcessShader(const std::string& shaderPath) = 0;

			// --------- Pipeline Configuration Settings ----------

			// Sets the primitive topology mode for this render pass.
			virtual RenderPass& SetTopology(const PrimitiveTopology& topology) = 0;

			// Sets the polygon mode for this render pass.
			virtual RenderPass& SetPolygonMode(const PolygonMode& mode) = 0;

			// Sets the triangle index order for this render pass. This determines which direction
			// a face is 'facing'.
			virtual RenderPass& SetFrontFace(const FrontFace& frontFace) = 0;

			// Sets the blend state for this render pass. This can be used to selectively allow/disallow
			// specific color masks.
			virtual RenderPass& SetBlendState(const bool enabled, const std::vector<ColorComponent>& masks = {}) = 0;

			// Enables or disables depth testing for this render pass.
			virtual RenderPass& EnableDepthTesting(const bool enabled = false) = 0;

			// Enables or disabled depth writing for this render pass.
			virtual RenderPass& EnableDepthWriting(const bool enabled = false) = 0;
		};
	}
}