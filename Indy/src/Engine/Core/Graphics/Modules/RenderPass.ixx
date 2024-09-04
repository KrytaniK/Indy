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
			enum ProcessType { PreProcess = 0, MainProcess, PostProcess };

		public:
			virtual ~RenderPass() = default;

			virtual const std::string& GetAlias() = 0;
			virtual const uint32_t& GetID() = 0;

			// Begins the configuration chain for this render pass
			virtual RenderPass& Begin() = 0;

			// Adds a configurable pipeline process to this render pass
			virtual RenderPass& AddProcess(const ProcessType& type) = 0;

			virtual RenderPass& BindShader(const PipelineShaderStage& shaderStage, const std::string& path) = 0;

			// Ends the configuration chain for this render pass
			virtual void End() = 0;

			// --------- Pipeline Process Configuration Settings ----------

			// Sets the primitive topology mode for the current pipeline process.
			virtual RenderPass& SetTopology(const PrimitiveTopology& topology) = 0;

			// Sets the polygon mode for the current pipeline process.
			virtual RenderPass& SetPolygonMode(const PolygonMode& mode) = 0;

			// Sets the triangle index order for the current pipeline process. This determines which direction
			// a face is 'facing'.
			virtual RenderPass& SetFrontFace(const FrontFace& frontFace) = 0;

			// Sets the blend state for the current pipeline process. This can be used to selectively allow/disallow
			// specific color masks.
			virtual RenderPass& SetBlendState(const bool enabled, const std::vector<ColorComponent>& masks = {}) = 0;

			// Enables or disables depth testing for the current pipeline process.
			virtual RenderPass& EnableDepthTesting(const bool enabled = false) = 0;

			// Enables or disabled depth writing for the current pipeline process.
			virtual RenderPass& EnableDepthWriting(const bool enabled = false) = 0;
		};
	}
}