#include <string>
#include <memory>
#include <sstream>

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
	// ----- Scope Profiler -----
	// --------------------------

	ScopeProfiler::ScopeProfiler(const std::string& scope_sig)
		: m_Scope(scope_sig), m_StartTime(Time::Now()) {}

	ScopeProfiler::~ScopeProfiler()
	{
		ProfileSessionManager::RecordProfile({m_Scope, Indy::Time::MilliDuration(m_StartTime, Time::Now())});
	}

	// -----------------------------------
	// ----- Profile Session Manager -----
	// -----------------------------------

	std::shared_ptr<ProfileSession> ProfileSessionManager::s_CurrentSession;

	bool ProfileSessionManager::SessionExists()
	{
		return ProfileSessionManager::s_CurrentSession != nullptr;
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
		//INDY_CORE_WARN("Profile Session Constructor!");
	}

	ProfileSession::~ProfileSession()
	{
		//INDY_CORE_WARN("Profile Session Destructor!");
	}

	void ProfileSession::RecordProfileResult(const ProfileResult& result)
	{
		//m_ProfileStream << "\n" << result.scope << " " << result.duration;
		INDY_CORE_INFO("Profile Result: [{0}] took {1}ms", result.scope, result.duration);
	}

	void ProfileSession::ReadStream()
	{
		/*ProfileResult result;
		while (m_ProfileStream >> result.scope >> result.duration)
		{
			INDY_CORE_WARN("Reading profile result for [{0}]: {1}ms", result.scope, result.duration);
		}*/
	}
}