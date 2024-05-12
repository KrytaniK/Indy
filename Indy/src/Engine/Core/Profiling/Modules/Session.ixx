module;

#include <memory>
#include <string>
#include <fstream>

export module Indy.Profiler:Session;

export
{
	namespace Indy
	{
		class Profiler;
		struct ProfileResult;

		class ProfileSession
		{
		public:
			ProfileSession(const std::string& name, const std::string& outputPath, const uint32_t maxRecordSize);
			~ProfileSession();

			const std::string& GetName();

			void RecordProfileResult(const ProfileResult& result);

		private:
			void InternalWriteStreamPrefix();
			void InternalWriteStreamSuffix();

		private:
			std::string m_Name;
			std::ofstream m_ProfilerOutput;
			uint8_t m_RecordCount;
			uint32_t m_MaxRecordCount;
			const std::string m_OutputPath;
		};
		
	}
}