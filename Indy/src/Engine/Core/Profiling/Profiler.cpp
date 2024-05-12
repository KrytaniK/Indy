#include <Engine/Core/LogMacros.h>

import Indy.Profiler;

namespace Indy
{
	Profiler::Profiler()
	{
		m_CurrentSession = std::make_shared<ProfileSession>("Default Session Name", m_OutputFilePath, m_MaxRecordCount);
		BeginSession("Default Session Name");
	}

	Profiler::Profiler(const std::string& sessionName)
	{
		m_CurrentSession = std::make_shared<ProfileSession>(sessionName, m_OutputFilePath, m_MaxRecordCount);
		BeginSession(sessionName);
	}

	Profiler::~Profiler()
	{
		EndSession();
	}

	void Profiler::SetOutputPath(const std::string& path)
	{
		const std::string& sessionName = m_CurrentSession->GetName();
		EndSession();
		m_OutputFilePath = path;
		BeginSession(sessionName);
	}

	const std::string& Profiler::GetOutputPath() { return m_OutputFilePath; }
	void Profiler::SetMaxRecordCount(const uint32_t& newCount) { m_MaxRecordCount = newCount; };
	const uint32_t& Profiler::GetMaxRecordCount() { return m_MaxRecordCount; };
	bool Profiler::IsInSession() const { return m_CurrentSession != nullptr; };
	const ProfileSession& Profiler::GetSession() { return *m_CurrentSession; };

	void Profiler::BeginSession(const std::string& name)
	{
		m_CurrentSession = std::make_shared<ProfileSession>(name, m_OutputFilePath, m_MaxRecordCount);
	}

	void Profiler::EndSession()
	{
		m_CurrentSession.reset();
	}


}