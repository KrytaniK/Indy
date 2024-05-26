#include <Engine/Core/LogMacros.h>

#include <fstream>

#include <string>
#include <vector>
#include <cstdint>

#include <shaderc/shaderc.hpp>

/*	NOTE: When writing SPIR-V to a file, if the out_file_path includes intermediary folders that do not exist yet,
 *		the operation will fail. the standard file system (introduced in C++17) offers
 *		a solution for this. It would be wise to implement.
 *
 *	TODO: Implement std::filesystem for better file handling
 */

import Indy.Graphics;

namespace Indy::Graphics
{
	std::string Shader::ReadGLSLShaderFromFile(const std::string& file_path)
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

	// I might extend this to support defining macros for the shader if needed.
	std::vector<uint32_t> Shader::CompileGLSLToSPIRV(const std::string& source, const std::string& name, ShaderType type, bool optimize)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		shaderc_shader_kind kind = shaderc_vertex_shader;
		switch(type)
		{
			case ShaderType::Vertex:
			{
				kind = shaderc_glsl_vertex_shader;
				break;
			}
			case ShaderType::Fragment:
			{
				kind = shaderc_glsl_fragment_shader;
				break;
			}
			case ShaderType::Compute:
			{
				kind = shaderc_glsl_compute_shader;
				break;
			}
			case ShaderType::Geometry:
			{
				kind = shaderc_glsl_geometry_shader;
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

	void Shader::WriteSPIRVToFile(const std::vector<uint32_t>& spirv, const std::string& out_file_path)
	{
		std::ofstream outFile(out_file_path, std::ios::out | std::ios::binary);

		if (!outFile.is_open())
		{
			INDY_CORE_ERROR("Could not open output file [{0}].", out_file_path);
			return;
		}

		outFile.write(reinterpret_cast<const char*>(spirv.data()), spirv.size() * sizeof(uint32_t));
		outFile.close();
	}

	SPIRV_Data Shader::ReadSPIRVFromFile(const std::string& file_path)
	{
		std::ifstream inSPIRV(file_path, std::ios::ate | std::ios::binary);

		if (!inSPIRV.is_open())
		{
			INDY_CORE_ERROR("Could not open file path [{0}].", file_path);
			return {0, nullptr, {}};
		}

		SPIRV_Data data;

		data.size = inSPIRV.tellg();
		data.binary = std::vector<char>(data.size);

		inSPIRV.seekg(0);
		inSPIRV.read(data.binary.data(), data.size);

		inSPIRV.close();

		data.code = reinterpret_cast<const uint32_t*>(data.binary.data());

		return data;
	}
}
