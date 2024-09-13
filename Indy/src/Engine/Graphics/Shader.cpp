#include <Engine/Core/LogMacros.h>

#include <filesystem>
#include <fstream>

#include <string>

import Indy.Graphics;

namespace Indy
{
	void WriteShaderToDisk(const Shader& shader, const size_t& size)
	{
		// Create intermediary directories if necessary
		if (!std::filesystem::exists(shader.path))
			std::filesystem::create_directories(shader.path);


		// Construct File name
		std::string outPath = shader.path + shader.name + GetShaderExtension(shader);

		std::ofstream stream(outPath, std::ios::out | std::ios::binary);

		if (!stream.is_open())
		{
			INDY_CORE_ERROR("Error writing shader to disk. Path does not exist! [{0}]", outPath);
			stream.close();
			return;
		}

		stream.write(reinterpret_cast<const char*>(shader.rawContent), size);
		stream.close();
	}

	void ReadShader(const std::string& path, Shader* outShader)
	{
		// Open the file
		std::ifstream stream(path, std::ios::in | std::ios::binary | std::ios::ate);
		if (!stream.is_open())
		{
			INDY_CORE_ERROR("Error loading shader. File/Directory does not exist! [{0}]", path);
			stream.close();
			return;
		}

		// Copy file to shader source string
		size_t size = stream.tellg();
		stream.seekg(0, std::ios::beg);

		outShader->fileContent = std::string(size, '\0');
		stream.read(outShader->fileContent.data(), size);
		outShader->rawContent = outShader->fileContent.data();

		stream.close();

		// Extract file name from path
		for (size_t i = path.size(); i >= 0; i--)
		{
			if (path[i] == '/')
			{
				outShader->name = path.substr(i + 1, path.size() - (i + 1));
				break;
			}
		}

		// Extract format and type from path
		outShader->format = GetShaderFormat(outShader->name);
		outShader->type = GetShaderType(outShader->name);
	}

	std::string GetShaderExtension(const Shader& shader)
	{
		std::string extension = "";

		switch (shader.format)
		{
			case INDY_SHADER_FORMAT_GLSL:			extension += ".glsl"; break;
			case INDY_SHADER_FORMAT_SPIR_V:			return ".spv";
		}

		switch (shader.type)
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

	ShaderFormat GetShaderFormat(const std::string& fileName)
	{
		if (fileName.find("glsl") != std::string::npos)
			return INDY_SHADER_FORMAT_GLSL;

		if (fileName.find("spv") != std::string::npos)
			return INDY_SHADER_FORMAT_SPIR_V;

		return INDY_SHADER_FORMAT_MAX_ENUM;
	}

	ShaderType GetShaderType(const std::string& fileName)
	{
		if (fileName.find("comp") != std::string::npos)
			return INDY_SHADER_TYPE_COMPUTE;

		if (fileName.find("vert") != std::string::npos)
			return INDY_SHADER_TYPE_VERTEX;

		if (fileName.find("frag") != std::string::npos)
			return INDY_SHADER_TYPE_FRAGMENT;

		if (fileName.find("geom") != std::string::npos)
			return INDY_SHADER_TYPE_GEOMETRY;

		if (fileName.find("tesc") != std::string::npos)
			return INDY_SHADER_TYPE_TESS_CONTROL;

		if (fileName.find("tese") != std::string::npos)
			return INDY_SHADER_TYPE_TESS_EVAL;

		return INDY_SHADER_TYPE_MAX_ENUM;
	}

	// ---------------------------------------------------------------------------------
	// OLD IMPL ------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------

	//void Shader::CompileSPIRV()
	//{
	//	shaderc::Compiler compiler;
	//	shaderc::CompileOptions options;

	//	options.SetOptimizationLevel(shaderc_optimization_level_performance);

	//	switch(m_Format)
	//	{
	//		case INDY_SHADER_FORMAT_GLSL: { CompileFromGLSL(compiler, options); break; }
	//		default: { return; }
	//	}

	//	m_SPIRV.size = m_SPIRV_Raw.size() * sizeof(uint32_t);
	//	m_SPIRV.data = m_SPIRV_Raw.data();
	//}

	//std::string Shader::GetSPIRVSource() const
	//{
	//	if (m_SPIRV.size == 0)
	//		return {};

	//	std::stringstream stream;
	//	stream.write(reinterpret_cast<const char*>(m_SPIRV.data), m_SPIRV.size);
	//	return stream.str();
	//}

	//void Shader::Reflect(const ShaderFormat& format)
	//{
	//	switch(format)
	//	{
	//		case INDY_SHADER_FORMAT_GLSL:
	//			{
	//				ReflectGLSL();
	//				break;
	//			}
	//		default:
	//			break;
	//	}
	//}

	//bool Shader::AssertShaderExtension(const std::string& fileName)
	//{
	//	switch (m_Format)
	//	{
	//		case INDY_SHADER_FORMAT_GLSL:
	//		{
	//			if (fileName.find("glsl") == std::string::npos) // Ensure GLSL shaders include "glsl" in the file name
	//			{
	//				INDY_CORE_ERROR("Error loading GLSL shader. Shader must contain 'glsl' in the file name. [{0}]", fileName);
	//				return false;
	//			}
	//			break;
	//		}
	//		case INDY_SHADER_FORMAT_SPIR_V:
	//		{
	//			if (!fileName.ends_with(".spv"))
	//			{
	//				INDY_CORE_ERROR("Error loading SPIR-V shader. Shader must contain the '.spv' file extension. [{0}]", fileName);
	//				return false;
	//			}
	//			break;
	//		}
	//	default:
	//		{
	//			INDY_CORE_ERROR("Error loading shader. Unknown format");
	//			return false;
	//		}
	//	}

	//	return true;
	//}

	//void Shader::CompileFromGLSL(const shaderc::Compiler& compiler, shaderc::CompileOptions& options)
	//{
	//	options.SetSourceLanguage(shaderc_source_language_glsl);
	//	shaderc::SpvCompilationResult spvModule = compiler.CompileGlslToSpv(
	//		m_Source, GetShaderCType(m_Type), m_FileName.c_str(), options
	//	);

	//	if (spvModule.GetCompilationStatus() != shaderc_compilation_status_success) {
	//		INDY_CORE_ERROR("GLSL COMPILATION ERROR: {0}", spvModule.GetErrorMessage());
	//		return;
	//	}

	//	m_SPIRV_Raw = std::vector<uint32_t>(spvModule.begin(), spvModule.end());
	//}

	//void Shader::ReflectGLSL()
	//{

	//}

	//shaderc_shader_kind Shader::GetShaderCType(const ShaderType& type)
	//{
	//	switch (type)
	//	{
	//		case INDY_SHADER_TYPE_VERTEX: return shaderc_glsl_vertex_shader;
	//		case INDY_SHADER_TYPE_FRAGMENT: return shaderc_glsl_fragment_shader;
	//		case INDY_SHADER_TYPE_COMPUTE: return shaderc_glsl_compute_shader;
	//		case INDY_SHADER_TYPE_GEOMETRY: return shaderc_glsl_geometry_shader;
	//		default: return shaderc_vertex_shader;
	//	}
	//}
}
