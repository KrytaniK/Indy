module;

#include <string>

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

		/*class Shader
		{
		public:
			static void WriteToDisk(const std::string& shaderName, const std::string& shaderSource, const ShaderFormat& format, const ShaderType& type = INDY_SHADER_TYPE_MAX_ENUM);
			static std::string GetFileExtension(const ShaderType& type, const ShaderFormat& format);

		public:
			Shader() {};
			Shader(const ShaderType& type, const ShaderFormat& format, const std::string& filePath);
			~Shader() = default;

			const ShaderType& GetType() const { return m_Type; };
			const ShaderFormat& GetFormat() const { return m_Format; };
			const std::string& GetSource() const { return m_Source; };
			const SPIRV& GetSPIRV() const { return m_SPIRV; };
			std::string GetSPIRVSource() const;

			void CompileSPIRV();

			void Reflect(const ShaderFormat& format);

		private:
			bool AssertShaderExtension(const std::string& path);

			void CompileFromGLSL(const shaderc::Compiler& compiler, shaderc::CompileOptions& options);

			void ReflectGLSL();

			shaderc_shader_kind GetShaderCType(const ShaderType& type);

		private:
			ShaderType m_Type;
			ShaderFormat m_Format;
			std::string m_filePath;
			std::string m_FileName;
			std::string m_Source;
			SPIRV m_SPIRV;
			std::vector<uint32_t> m_SPIRV_Raw;
		};*/
	}
}