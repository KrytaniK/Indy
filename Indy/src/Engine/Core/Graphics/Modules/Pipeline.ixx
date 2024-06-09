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
			INDY_PIPELINE_TYPE_RAYTRACE = 2
		} PipelineType;

		class Pipeline
		{
		public:
			Pipeline() = default;
			virtual ~Pipeline() = default;

			virtual const PipelineType& GetType() const = 0;

			virtual void BindShader(Shader& shader) = 0;

			virtual void Build() = 0;
		};
	}
}