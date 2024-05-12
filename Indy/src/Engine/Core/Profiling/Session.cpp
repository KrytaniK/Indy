#include <sstream>

import Indy.Profiler;

namespace Indy
{
	ProfileSession::ProfileSession(const std::string& name, const std::string& outputPath, const uint32_t maxRecordSize)
		: m_Name(name), m_MaxRecordCount(maxRecordSize), m_OutputPath(outputPath)
	{
		this->m_RecordCount = 0;
		this->m_ProfilerOutput.open(outputPath);
		this->InternalWriteStreamPrefix();
	}

	ProfileSession::~ProfileSession()
	{
		this->InternalWriteStreamSuffix();
		this->m_ProfilerOutput.close();
	}

	const std::string& ProfileSession::GetName() { return this->m_Name; };

	void ProfileSession::InternalWriteStreamPrefix()
	{
		this->m_ProfilerOutput << "{\n\t\"profiles\": [\n\t\t{}";
		this->m_ProfilerOutput.flush();
	}

	void ProfileSession::InternalWriteStreamSuffix()
	{
		this->m_ProfilerOutput << "\n]\n}";
		this->m_ProfilerOutput.flush();
	}

	void ProfileSession::RecordProfileResult(const ProfileResult& result)
	{
		// Reset the .json file if we're going to record more than 10kb of data.
		// This takes (on average) between 0.02-0.1ms.
		if (this->m_RecordCount >= m_MaxRecordCount)
		{
			this->m_RecordCount = 0;
			this->m_ProfilerOutput.close();
			this->m_ProfilerOutput.open(m_OutputPath);
			this->InternalWriteStreamPrefix();
		}

		// Convert Profile Result to JSON
		std::stringstream json;
		json << ",\n\t\t{ ";
		json << "\"scope\": \"";
		json << result.scope << "\", ";
		json << "\"duration\": \"";
		json << (result.duration.count() / 1e+6) << "\"";
		json << " }";

		// Write JSON to file and flush the stream
		this->m_ProfilerOutput << json.str();
		this->m_ProfilerOutput.flush();

		// Increment the number of records we have
		this->m_RecordCount++;
	}
}