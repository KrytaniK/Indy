module;

#include <memory>
#include <string>
#include <chrono>
#include <iostream>

export module Indy.Profiler;

export import :Session;

export
{
	namespace Indy
	{
		struct ProfileResult
		{
			std::string scope;
			std::chrono::nanoseconds duration;
		};

		class Profiler
		{
		public:
			Profiler();
			Profiler(const std::string& sessionName);
			~Profiler();

			void SetOutputPath(const std::string& path);
			const std::string& GetOutputPath();

			void SetMaxRecordCount(const uint32_t& newCount);
			const uint32_t& GetMaxRecordCount();

			void BeginSession(const std::string& name);
			void EndSession();

			bool IsInSession() const;
			const ProfileSession& GetSession();

			template<typename F, typename... Args>
			void ProfileFunc(const std::string& name, const F& func, Args... args);;

			template<typename C, typename F, typename... Args>
			void ProfileFunc(const std::string& name, C* instance, F(C::* func), Args... args);

		private:
			std::shared_ptr<ProfileSession> m_CurrentSession;
			std::string m_OutputFilePath = "./IndyEngine_DefaultProfilerOutput.txt";
			uint32_t m_MaxRecordCount = 200;
		};

		// Global Profiler Definitions

		Profiler s_Indy_Global_Profiler;

		template<typename F, typename... Args>
		void ProfileFunc(const std::string& name, const F& func, Args... args)
		{
			s_Indy_Global_Profiler.ProfileFunc(name, func, args...);
		}

		template<typename C, typename F, typename... Args>
		void ProfileFunc(const std::string& name, C* instance, F(C::* func), Args... args)
		{
			s_Indy_Global_Profiler.ProfileFunc(name, instance, func, args...);
		}

		// Template Definitions

		template<typename C, typename F, typename... Args>
		void Profiler::ProfileFunc(const std::string& name, C* instance, F(C::* func), Args... args)
		{
			auto start = std::chrono::steady_clock::now();
			(instance->*func)(args...);
			auto end = std::chrono::steady_clock::now();

			m_CurrentSession->RecordProfileResult({
				name,
				std::chrono::time_point_cast<std::chrono::nanoseconds>(end).time_since_epoch() - std::chrono::time_point_cast<std::chrono::nanoseconds>(start).time_since_epoch()
				});
		};

		template<typename F, typename... Args>
		void Profiler::ProfileFunc(const std::string& name, const F& func, Args... args)
		{
			auto start = std::chrono::steady_clock::now();
			func(args...);
			auto end = std::chrono::steady_clock::now();

			m_CurrentSession->RecordProfileResult({
				name,
				std::chrono::time_point_cast<std::chrono::nanoseconds>(end).time_since_epoch() - std::chrono::time_point_cast<std::chrono::nanoseconds>(start).time_since_epoch()
				});
		};
	}
}