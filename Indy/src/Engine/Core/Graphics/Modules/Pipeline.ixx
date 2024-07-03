module;

#include <string>

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
			INDY_PIPELINE_TYPE_RAYTRACE = 2,
			INDY_PIPELINE_TYPE_MAX_ENUM = 3
		} PipelineType;

		typedef enum PipelineShaderStage 
		{
			INDY_PIPELINE_SHADER_STAGE_COMPUTE = 0,
			INDY_PIPELINE_SHADER_STAGE_VERTEX = 1,
			INDY_PIPELINE_SHADER_STAGE_FRAGMENT = 2,
			INDY_PIPELINE_SHADER_STAGE_GEOMETRY = 3,
			INDY_PIPELINE_SHADER_STAGE_TESS_CONTROL = 4,
			INDY_PIPELINE_SHADER_STAGE_TESS_EVAL = 5,
			INDY_PIPELINE_SHADER_STAGE_MAX_ENUM = 6
		} PipelineShaderStage;

		// Tag structure for refined pipeline building, per API
		struct PipelineBuildOptions 
		{
			PipelineType type = INDY_PIPELINE_TYPE_MAX_ENUM;
		};

		// Tag Structure for build results
		struct PipelineBuildResult {};

		class PipelineBuilder
		{
		public:
			PipelineBuilder() = default;
			virtual ~PipelineBuilder() = default;

			virtual void Build(PipelineBuildOptions* options) = 0;

			virtual void Clear() = 0;

			virtual void BindShader(const std::string& shaderPath) = 0;
			virtual void BindShader(const Shader& shader, const PipelineShaderStage& stage) = 0;
		};
	}
}