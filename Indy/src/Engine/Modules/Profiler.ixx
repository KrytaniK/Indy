module;

#include <chrono>
#include <string>
#include <memory>
#include <unordered_map>
#include <fstream>

export module Indy_Core:Profiling;

/* Note: This approach is NOT thread safe! This needs to be accounted for!
*	Should be simple to implement, though.
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

		const std::string PROFILE_OUTPUT_PATH = "Indy_Profiling_Output.json";
		const uint8_t MAX_PROFILE_RECORD_COUNT = 200;

		class ProfileSession
		{
		private:
			std::ofstream m_ProfilerOutput;
			uint8_t m_RecordCount;

		public:
			std::string name;

		public:
			ProfileSession(const std::string& name); // Starts a new thread 
			~ProfileSession(); // Closes the thread

			void RecordProfileResult(const ProfileResult& result);
			void ReadStream();

		private:
			void InternalWriteStreamPrefix();
			void InternalWriteStreamSuffix();
		};

		class ProfileSessionManager
		{
		private:
			static std::shared_ptr<ProfileSession> s_CurrentSession;

		public:
			static void RegisterSession(std::shared_ptr<ProfileSession> session);
			static void EndSession();

			static bool SessionExists();
			static const std::string& GetSessionName();

			static void RecordProfile(const ProfileResult& result);
		};
	}

	std::unique_ptr<Indy::ScopeProfiler> ProfileScope(const std::string& scope_sig);

	std::shared_ptr<Indy::ProfileSession> StartProfilingSession(const std::string& sessionName);
	void EndProfilingSession();

	std::ostream& operator<<(std::ostream& os, const Indy::ProfileResult& profileResult);
	std::istream& operator>>(std::istream& os, const Indy::ProfileResult& profileResult);
}