#include <Engine/Core/LogMacros.h>

#include <filesystem>
#include <fstream>
#include <sstream>

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

namespace Indy
{
	void Shader::WriteToDisk(const std::string& shaderName, const std::string& shaderSource, const ShaderFormat& format, const ShaderType& type)
	{
		// Create intermediary directories if necessary
		if (!std::filesystem::exists("shaders"))
			std::filesystem::create_directories("shaders");


		// Construct File name
		std::string file = shaderName + GetFileExtension(type, format);
		std::string filePath = "shaders/" + file;

		std::ofstream stream(filePath, std::ios::out | std::ios::binary);

		if (!stream.is_open())
		{
			INDY_CORE_ERROR("Error writing shader to disk. Path does not exist! [{0}]", filePath);
			stream.close();
			return;
		}

		stream.write(shaderSource.data(), shaderSource.size());
		stream.close();
	}

	std::string Shader::GetFileExtension(const ShaderType& type, const ShaderFormat& format)
	{
		std::string extension = "";

		switch(format)
		{
			case INDY_SHADER_FORMAT_GLSL:			extension += ".glsl"; break;
			case INDY_SHADER_FORMAT_SPIR_V:			extension += ".spv";  break;
		}

		switch (type)
		{
			case INDY_SHADER_TYPE_VERTEX:			extension += ".vert"; break;
			case INDY_SHADER_TYPE_FRAGMENT:			extension += ".frag"; break;
			case INDY_SHADER_TYPE_COMPUTE:			extension += ".comp"; break;
			case INDY_SHADER_TYPE_GEOMETRY:			extension += ".geom"; break;
			case INDY_SHADER_TYPE_TESS_CONTROL:		extension += ".tesc"; break;
			case INDY_SHADER_TYPE_TESS_EVAL:		extension += ".tese"; break;
			default: break;
		}

		return extension;
	}

	Shader::Shader(const ShaderType& type, const ShaderFormat& format, const std::string& filePath)
		: m_Type(type), m_Format(format), m_filePath(filePath), m_SPIRV({})
	{
		// Get File Name
		size_t slashIndex = 0, i = 0;
		for (const auto& c : filePath)
		{
			if (c == '/')
				slashIndex = i + 1;
			i++;
		}

		// Get the file name and assert file naming conventions are used
		m_FileName = filePath.substr(slashIndex, filePath.size() - slashIndex);
		if (!AssertShaderExtension(m_FileName))
			return;

		// Open the file
		std::ifstream stream(filePath, std::ios::in | std::ios::binary | std::ios::ate);
		if (!stream.is_open())
		{
			// If the file doesn't exist, warn the user, but prepare the shader for writing.
			// Conversion should not be possible, as there is no source.
			// Basically, treat as an empty shader.

			INDY_CORE_ERROR("Error loading shader. File/Directory does not exist! [{0}]", filePath);
			stream.close();
			return;
		}

		// Copy file to shader source string
		size_t size = stream.tellg();
		stream.seekg(0, std::ios::beg);

		m_Source = std::string(size, '\0');
		stream.read(m_Source.data(), size);

		stream.close();
	}

	void Shader::CompileSPIRV()
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		options.SetOptimizationLevel(shaderc_optimization_level_performance);

		switch(m_Format)
		{
			case INDY_SHADER_FORMAT_GLSL: { CompileFromGLSL(compiler, options); break; }
			default: { return; }
		}

		m_SPIRV.size = m_SPIRV_Raw.size() * sizeof(uint32_t);
		m_SPIRV.data = m_SPIRV_Raw.data();
	}

	std::string Shader::GetSPIRVSource() const
	{
		if (m_SPIRV.size == 0)
			return {};

		std::stringstream stream;
		stream.write(reinterpret_cast<const char*>(m_SPIRV.data), m_SPIRV.size);
		return stream.str();
	}

	void Shader::Reflect(const ShaderFormat& format)
	{
		switch(format)
		{
			case INDY_SHADER_FORMAT_GLSL:
				{
					ReflectGLSL();
					break;
				}
			default:
				break;
		}
	}

	bool Shader::AssertShaderExtension(const std::string& fileName)
	{
		switch (m_Format)
		{
			case INDY_SHADER_FORMAT_GLSL:
			{
				if (fileName.find("glsl") == std::string::npos) // Ensure GLSL shaders include "glsl" in the file name
				{
					INDY_CORE_ERROR("Error loading GLSL shader. Shader must contain 'glsl' in the file name. [{0}]", fileName);
					return false;
				}
				break;
			}
			case INDY_SHADER_FORMAT_SPIR_V:
			{
				if (!fileName.ends_with(".spv"))
				{
					INDY_CORE_ERROR("Error loading SPIR-V shader. Shader must contain the '.spv' file extension. [{0}]", fileName);
					return false;
				}
				break;
			}
		default:
			{
				INDY_CORE_ERROR("Error loading shader. Unknown format");
				return false;
			}
		}

		return true;
	}

	void Shader::CompileFromGLSL(const shaderc::Compiler& compiler, shaderc::CompileOptions& options)
	{
		options.SetSourceLanguage(shaderc_source_language_glsl);
		shaderc::SpvCompilationResult spvModule = compiler.CompileGlslToSpv(
			m_Source, GetShaderCType(m_Type), m_FileName.c_str(), options
		);

		if (spvModule.GetCompilationStatus() != shaderc_compilation_status_success) {
			INDY_CORE_ERROR("GLSL COMPILATION ERROR: {0}", spvModule.GetErrorMessage());
			return;
		}

		m_SPIRV_Raw = std::vector<uint32_t>(spvModule.begin(), spvModule.end());
	}

	void Shader::ReflectGLSL()
	{

	}

	shaderc_shader_kind Shader::GetShaderCType(const ShaderType& type)
	{
		switch (type)
		{
			case INDY_SHADER_TYPE_VERTEX: return shaderc_glsl_vertex_shader;
			case INDY_SHADER_TYPE_FRAGMENT: return shaderc_glsl_fragment_shader;
			case INDY_SHADER_TYPE_COMPUTE: return shaderc_glsl_compute_shader;
			case INDY_SHADER_TYPE_GEOMETRY: return shaderc_glsl_geometry_shader;
			default: return shaderc_vertex_shader;
		}
	}
}

//
//	void Shader::WriteSPIRVToFile(const std::vector<uint32_t>& spirv, const std::string& out_file_path)
//	{
//		std::ofstream outFile(out_file_path, std::ios::out | std::ios::binary);
//
//		if (!outFile.is_open())
//		{
//			INDY_CORE_ERROR("Could not open output file [{0}].", out_file_path);
//			return;
//		}
//
//		outFile.write(reinterpret_cast<const char*>(spirv.data()), spirv.size() * sizeof(uint32_t));
//		outFile.close();
//	}
//
//	SPIRV_Data Shader::ReadSPIRVFromFile(const std::string& file_path)
//	{
//		std::ifstream inSPIRV(file_path, std::ios::ate | std::ios::binary);
//
//		if (!inSPIRV.is_open())
//		{
//			INDY_CORE_ERROR("Could not open file path [{0}].", file_path);
//			return {0, nullptr, {}};
//		}
//
//		SPIRV_Data data;
//
//		data.size = inSPIRV.tellg();
//		data.binary = std::vector<char>(data.size);
//
//		inSPIRV.seekg(0);
//		inSPIRV.read(data.binary.data(), data.size);
//
//		inSPIRV.close();
//
//		data.code = reinterpret_cast<const uint32_t*>(data.binary.data());
//
//		return data;
//	}
//}
