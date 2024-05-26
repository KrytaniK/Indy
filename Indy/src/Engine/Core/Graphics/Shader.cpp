#include <Engine/Core/LogMacros.h>

#include <fstream>

#include <string>
#include <vector>
#include <cstdint>

#include <shaderc/shaderc.hpp>

import Indy.Graphics;

namespace Indy::Graphics
{
	std::string Shader::ReadGLSLShaderFromFile(const ::std::string& file_path)
	{
		std::ifstream inGLSL(file_path, std::ios::in | std::ios::binary | std::ios::ate);

		if (!inGLSL.is_open())
		{
			INDY_CORE_ERROR("Could not open file path [{0}].", file_path);
			return "";
		}

		size_t fileSize = inGLSL.tellg();
		inGLSL.seekg(0, std::ios::beg);

		std::string content(fileSize, '\0');
		inGLSL.read(&content[0], fileSize);
		inGLSL.close();

		return content;
	}

	std::vector<uint32_t> Shader::CompileGLSLToSPRV(const std::string& source, const std::string& name, ShaderType type)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		shaderc_shader_kind kind;
		switch(type)
		{
			case ShaderType::Vertex:
			{
				kind = shaderc_vertex_shader;
				break;
			}
			case ShaderType::Fragment:
			{
				kind = shaderc_fragment_shader;
				break;
			}
			case ShaderType::Compute:
			{
				kind = shaderc_compute_shader;
				break;
			}
			case ShaderType::Geometry:
			{
				kind = shaderc_geometry_shader;
				break;
			}
		}

		shaderc::SpvCompilationResult spvModule = compiler.CompileGlslToSpv(source, kind, name.c_str(), options);

		if (spvModule.GetCompilationStatus() != shaderc_compilation_status_success) {
			INDY_CORE_ERROR("{0}", spvModule.GetErrorMessage());
			return std::vector<uint32_t>();
		}

		return {spvModule.begin(), spvModule.end()};
	}

	void Shader::WriteSPRVToFile(const std::vector<uint32_t>& spirv, const std::string& outPath)
	{
		std::ofstream outFile(outPath, std::ios::binary);
		if (!outFile.is_open())
		{
			INDY_CORE_ERROR("Could not open output file [{0}].", outPath);
			return;
		}

		outFile.write(reinterpret_cast<const char*>(spirv.data()), spirv.size() * sizeof(uint32_t));
		outFile.close();
	}

}