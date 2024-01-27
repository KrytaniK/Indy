module;

#include <chrono>
#include <string>
#include <memory>
#include <unordered_map>
#include <sstream>

export module Indy_Core:Profiling;

/*	Scope Profiler pushes profile results to session results stream
* 
*	Session Manager Periodically checks the current stream and reads
*	profile data.
*		- Stores a raw map of profile results "std::unordered_map<std::string, std::vector<ProfileResult>> results"
*		- For each entry, takes the average and stores the average result in an internal map
*			"std::unordered_map<std::string, ProfileResult> avgResults;"
*/

/* TODO
	- Create PROFILE_SCOPE macro. need to force variable creation to actually profile scopes.
*/

export
{
	namespace Indy
	{
		class ScopeProfiler
		{
		public:
			ScopeProfiler(const std::string& scope_sig);
			~ScopeProfiler();

		private:
			std::string m_Scope;
			std::chrono::high_resolution_clock::time_point m_StartTime;
		};

		struct ProfileResult
		{
			std::string scope;
			double duration;
		};

		class ProfileSession
		{
		private:
			std::stringstream m_ProfileStream;

		public:
			std::string name;

		public:
			ProfileSession(const std::string& name); // Starts a new thread 
			~ProfileSession(); // Closes the thread

			void RecordProfileResult(const ProfileResult& result);
			void ReadStream();
		};

		class ProfileSessionManager
		{
		private:
			static std::shared_ptr<ProfileSession> s_CurrentSession;

		public:
			static void RegisterSession(std::shared_ptr<ProfileSession> session);
			static void EndSession();
			static bool SessionExists();

			static void RecordProfile(const ProfileResult& result);
		};
	}

	std::unique_ptr<Indy::ScopeProfiler> ProfileScope(const std::string& scope_sig);

	std::shared_ptr<Indy::ProfileSession> StartProfilingSession(const std::string& sessionName);
	void EndProfilingSession();

	std::ostream& operator<<(std::ostream& os, const Indy::ProfileResult& profileResult);
	std::istream& operator>>(std::istream& os, const Indy::ProfileResult& profileResult);
}