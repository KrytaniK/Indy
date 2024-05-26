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

		struct SPIRV_Data
		{
			size_t size;
			const uint32_t* code;
			std::vector<char> binary;
		};

		class Shader
		{
		public:
			static std::string ReadGLSLShaderFromFile(const std::string& file_path);
			// Modified version of https://github.com/google/shaderc/blob/main/examples/online-compile/main.cc#L80
			static std::vector<uint32_t> CompileGLSLToSPIRV(const std::string& source, const std::string& name, ShaderType type, bool optimize = false);
			static void WriteSPIRVToFile(const std::vector<uint32_t>& spirv, const std::string& outPath);
			static SPIRV_Data ReadSPIRVFromFile(const std::string& file_path);

		};
	}
}