module;

#include <any>

export module Indy.Graphics:Pipeline;

import :Shader;

export
{
	namespace Indy
	{
		typedef enum PipelineType
		{
			INDY_PIPELINE_TYPE_COMPUTE = 0,
			INDY_PIPELINE_TYPE_GRAPHICS = 1,
			INDY_PIPELINE_TYPE_RAY_TRACING = 2
		} PipelineType;

		typedef enum PipelineShaderStage
		{
			INDY_PIPELINE_SHADER_STAGE_COMPUTE = 0,
			INDY_PIPELINE_SHADER_STAGE_VERTEX = 1,
			INDY_PIPELINE_SHADER_STAGE_TESS_CONTROL = 2,
			INDY_PIPELINE_SHADER_STAGE_TESS_EVAL = 3,
			INDY_PIPELINE_SHADER_STAGE_GEOMETRY = 4,
			INDY_PIPELINE_SHADER_STAGE_FRAGMENT = 5,
		} PipelineShaderStage;

		class Pipeline
		{
		public:
			enum ShaderStage { Compute, Vertex, Fragment };

		public:
			Pipeline() = default;
			virtual ~Pipeline() = default;

			virtual void BindShader(const PipelineShaderStage& stage, Shader& shader) = 0;

			virtual void Build() = 0;
		};
	}
}