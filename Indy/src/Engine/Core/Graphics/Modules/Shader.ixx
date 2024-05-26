module;

#include <string>
#include <vector>
#include <cstdint>

#include <shaderc/shaderc.hpp>

export module Indy.Graphics:Shader;

export
{
	namespace Indy::Graphics
	{
		enum class ShaderType { Vertex, Fragment, Compute, Geometry };

		class Shader
		{
		public:
			static std::string ReadGLSLShaderFromFile(const std::string& file_path);
			static std::vector<uint32_t> CompileGLSLToSPRV(const std::string& source, const std::string& name, ShaderType type);
			static void WriteSPRVToFile(const std::vector<uint32_t>& spirv, const std::string& outPath);

		};
	}
}