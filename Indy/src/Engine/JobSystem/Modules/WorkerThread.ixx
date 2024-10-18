module;

#include <span>
#include <string>
#include <thread>
#include <mutex>

export module Indy.JobSystem:WorkerThread;

import Indy.Multithreading;
import Indy.Utility;

import :State;
import :Job;

export
{
	namespace Indy::JobSystem
	{
		class WorkerThread : public IThread
		{
		public:
			WorkerThread(const uint32_t& usageMask, const WorkerThreadState& initialState);
			virtual ~WorkerThread() override;

			virtual uint32_t GetUsage() override;

			virtual void Start() override;

			virtual void Sleep() override;
			
			virtual void Wake() override;

			virtual void Join() override;

			virtual void operator()() override;

			WorkerThread(const WorkerThread& other);

		private:
			void OnThreadIdle(const std::string& threadID, std::shared_ptr<WorkerThreadQueue>& localQueue);
			void OnThreadWork(const std::string& threadID, std::shared_ptr<WorkerThreadQueue>& localQueue);
			void OnThreadExit(const std::string& threadID, std::shared_ptr<WorkerThreadQueue>& localQueue);

		private:
			WorkerThreadState m_State;
			std::thread m_Thread;
			uint32_t m_UsageMask;
		};
	}
}