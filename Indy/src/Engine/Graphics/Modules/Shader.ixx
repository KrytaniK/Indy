module;

#include <string>
#include <sstream>

#include <shaderc/shaderc.hpp>

export module Indy.Graphics:Shader;

export
{
	namespace Indy
	{
		typedef enum ShaderType
		{
			INDY_SHADER_TYPE_COMPUTE = 0,
			INDY_SHADER_TYPE_VERTEX = 1,
			INDY_SHADER_TYPE_FRAGMENT = 2,
			INDY_SHADER_TYPE_GEOMETRY = 3,
			INDY_SHADER_TYPE_TESS_CONTROL = 4,
			INDY_SHADER_TYPE_TESS_EVAL = 5,
			INDY_SHADER_TYPE_MAX_ENUM = UINT32_MAX
		} ShaderType;

		typedef enum ShaderFormat
		{
			INDY_SHADER_FORMAT_GLSL = 0,
			INDY_SHADER_FORMAT_SPIR_V = 1,
			INDY_SHADER_FORMAT_MAX_ENUM = UINT32_MAX,
		} ShaderFormat;

		struct Shader
		{
			ShaderType type = INDY_SHADER_TYPE_MAX_ENUM;
			ShaderFormat format = INDY_SHADER_FORMAT_MAX_ENUM;
			std::string name = "";
			std::string path = "";
			std::string fileContent = "";
			void* rawContent = nullptr;
		};

		void WriteShaderToDisk(const Shader& shader, const size_t& size);

		void ReadShader(const std::string& path, Shader* outShader);

		std::string GetShaderExtension(const Shader& shader);

		ShaderFormat GetShaderFormat(const std::string& fileName);

		ShaderType GetShaderType(const std::string& fileName);
	}
}