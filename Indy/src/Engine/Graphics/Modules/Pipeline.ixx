module;

#include <string>

export module Indy.Graphics:Pipeline;

import :Shader;
import :Types;

export
{
	namespace Indy::Graphics
	{
		class PipelineBuilder
		{
		public:
			PipelineBuilder() = default;
			virtual ~PipelineBuilder() = default;

			//virtual void Build(PipelineBuildOptions* options) = 0;

			virtual void Clear() = 0;

			virtual void BindShader(const std::string& shaderPath) = 0;
			virtual void BindShader(const Shader& shader, const PipelineShaderStage& stage) = 0;
		};
	}
}