#include <Engine/Core/LogMacros.h>

#include <string>

import Indy_Core_Profiler;

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

	void Profiler::BeginSession(const std::string& name)
	{
		m_CurrentSession = std::make_shared<ProfileSession>(name, m_OutputFilePath, m_MaxRecordCount);
	}

	void Profiler::EndSession()
	{
		m_CurrentSession.reset();
	}


}