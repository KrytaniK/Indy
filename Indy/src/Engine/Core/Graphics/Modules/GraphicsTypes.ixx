module;

#include <vector>
#include <string>

export module Indy.Graphics:Types;

export
{
	namespace Indy::Graphics
	{
		typedef enum PipelineType
		{
			PIPELINE_TYPE_COMPUTE = 0,
			PIPELINE_TYPE_GRAPHICS = 1,
			PIPELINE_TYPE_RAYTRACE = 2,
			PIPELINE_TYPE_MAX_ENUM = 3
		} PipelineType;

		typedef enum PipelineShaderStage
		{
			PIPELINE_SHADER_STAGE_COMPUTE = 0,
			PIPELINE_SHADER_STAGE_VERTEX = 1,
			PIPELINE_SHADER_STAGE_FRAGMENT = 2,
			PIPELINE_SHADER_STAGE_GEOMETRY = 3,
			PIPELINE_SHADER_STAGE_TESS_CONTROL = 4,
			PIPELINE_SHADER_STAGE_TESS_EVAL = 5,
			PIPELINE_SHADER_STAGE_MAX_ENUM = 6
		} PipelineShaderStage;

		typedef enum PrimitiveTopology
		{
			PRIMITIVE_TOPOLOGY_POINT_LIST = 0,
			PRIMITIVE_TOPOLOGY_LINE_LIST = 1,
			PRIMITIVE_TOPOLOGY_LINE_STRIP = 2,
			PRIMITIVE_TOPOLOGY_TRIANGLE_LIST = 3,
			PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP = 4,
			PRIMITIVE_TOPOLOGY_TRIANGLE_FAN = 5,
			PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY = 6,
			PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY = 7,
			PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY = 8,
			PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY = 9,
			PRIMITIVE_TOPOLOGY_PATCH_LIST = 10,
			PRIMITIVE_TOPOLOGY_MAX_ENUM = 11
		} PrimitiveTopology;

		typedef enum PolygonMode
		{
			POLYGON_MODE_FILL = 0,
			POLYGON_MODE_LINE = 1,
			POLYGON_MODE_POINT = 2,
			POLYGON_MODE_MAX_ENUM = 3
		} PolygonMode;

		typedef enum CullMode
		{
			CULL_MODE_NONE = 0,
			CULL_MODE_FRONT = 1,
			CULL_MODE_BACK = 2,
			CULL_MODE_FRONT_AND_BACK = 3,
			CULL_MODE_MAX_ENUM = 4
		} CullMode;

		typedef enum FrontFace
		{
			FRONT_FACE_CLOCKWISE = 0,
			FRONT_FACE_COUNTER_CLOCKWISE = 1,
			FRONT_FACE_MAX_ENUM = 2
		} FrontFace;

		typedef enum ColorComponent
		{
			COLOR_COMPONENT_R = 0,
			COLOR_COMPONENT_G = 1,
			COLOR_COMPONENT_B = 2,
			COLOR_COMPONENT_A = 3
		} ColorComponent;

		struct BlendState
		{
			bool enabled = false;
			std::vector<ColorComponent> masks;
		};

		struct DepthStencilState
		{
			bool depthTestEnabled = false;
			bool depthWriteEnable = false;
		};

		struct PipelineState
		{
			PrimitiveTopology topology = PRIMITIVE_TOPOLOGY_MAX_ENUM;
			PolygonMode polygonMode = POLYGON_MODE_MAX_ENUM;
			CullMode cullMode = CULL_MODE_MAX_ENUM;
			FrontFace frontFace = FRONT_FACE_MAX_ENUM;
			BlendState blendState{};
			DepthStencilState depthStencilState{};
		};

		struct RenderPass
		{
			uint32_t id;
			std::string debugName;
			PipelineState pipelineState{};
		};

		struct Viewport
		{
			uint32_t id;
			std::string debugName;
			std::vector<uint32_t> renderPassIDs;
			bool enabled;
		};
	}
}