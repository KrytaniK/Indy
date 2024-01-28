#include <string>
#include <memory>
#include <sstream>
#include <fstream>

#include "Engine/Core/LogMacros.h"
#include "Engine/Core/ProfileMacros.h"

import Indy_Core;
import Time;

std::unique_ptr<Indy::ScopeProfiler> ProfileScope(const std::string& scope_sig)
{
	return std::make_unique<Indy::ScopeProfiler>(scope_sig);
}

std::shared_ptr<Indy::ProfileSession> StartProfilingSession(const std::string& sessionName)
{
	if (Indy::ProfileSessionManager::SessionExists())
		return nullptr;

	std::shared_ptr<Indy::ProfileSession> session = std::make_shared<Indy::ProfileSession>(sessionName);

	Indy::ProfileSessionManager::RegisterSession(session);

	return session;
}

std::ostream& operator<<(std::ostream& os, const Indy::ProfileResult& profileResult)
{
	return os;
};

std::istream& operator>>(std::istream& is, const Indy::ProfileResult& profileResult)
{
	return is;
};

void EndProfilingSession()
{
	Indy::ProfileSessionManager::EndSession();
}

namespace Indy
{
	// --------------------------
	// ----- Profile Result -----
	// --------------------------

	// --------------------------
	// ----- Scope Profiler -----
	// --------------------------

	ScopeProfiler::ScopeProfiler(const std::string& scope_sig)
		: m_Scope(scope_sig), m_StartTime(Time::Now()) {}

	ScopeProfiler::~ScopeProfiler()
	{
		auto end = Time::Now();
		ProfileSessionManager::RecordProfile({m_Scope, Indy::Time::MilliDuration(m_StartTime, end)});
	}

	// -----------------------------------
	// ----- Profile Session Manager -----
	// -----------------------------------

	std::shared_ptr<ProfileSession> ProfileSessionManager::s_CurrentSession;

	bool ProfileSessionManager::SessionExists()
	{
		return ProfileSessionManager::s_CurrentSession != nullptr;
	}

	const std::string& ProfileSessionManager::GetSessionName()
	{
		return ProfileSessionManager::s_CurrentSession->name;
	}

	void ProfileSessionManager::RecordProfile(const ProfileResult& result)
	{
		if (!ProfileSessionManager::SessionExists())
		{
			INDY_CORE_ERROR("Could not record profile for [{0}]: No valid session!", result.scope);
			return;
		}

		ProfileSessionManager::s_CurrentSession->RecordProfileResult(result);
	}

	void ProfileSessionManager::RegisterSession(std::shared_ptr<ProfileSession> session)
	{
		if (ProfileSessionManager::SessionExists())
		{
			INDY_CORE_ERROR("Could not start profiling session [{0}]: There's already a session open!", session->name);
			return;
		}

		ProfileSessionManager::s_CurrentSession = session;
	}

	void ProfileSessionManager::EndSession()
	{
		ProfileSessionManager::s_CurrentSession.reset();
	}

	// ---------------------------
	// ----- Profile Session -----
	// ---------------------------
	
	ProfileSession::ProfileSession(const std::string& name)
		: name(name)
	{
		this->m_RecordCount = 0;
		this->m_ProfilerOutput.open(PROFILE_OUTPUT_PATH);
		this->InternalWriteStreamPrefix();
	}

	ProfileSession::~ProfileSession()
	{
		this->InternalWriteStreamSuffix();

		this->m_ProfilerOutput.close();
	}

	void ProfileSession::RecordProfileResult(const ProfileResult& result)
	{
		// Reset the .json file if we're going to record more than 10kb of data.
		if (this->m_RecordCount >= MAX_PROFILE_RECORD_COUNT)
		{

			this->m_RecordCount = 0;
			this->m_ProfilerOutput.close();
			this->m_ProfilerOutput.open(PROFILE_OUTPUT_PATH);
			this->InternalWriteStreamPrefix();
		}

		std::stringstream json;

		json << ",\n\t\t{ ";
		json << "\"scope\": \"";
		json << result.scope << "\", ";
		json << "\"duration\": \"";
		json << result.duration << "\"";
		json << " }";

		this->m_ProfilerOutput << json.str();
		this->m_ProfilerOutput.flush();

		this->m_RecordCount++;
	}

	void ProfileSession::ReadStream()
	{
		
	}

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
}