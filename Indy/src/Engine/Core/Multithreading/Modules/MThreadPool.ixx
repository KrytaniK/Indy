module;

#include <cstdint>
#include <vector>
#include <thread>
#include <memory>

export module Indy.Multithreading:ThreadPool;

import :Atomic;
import :Thread;

export
{
	namespace Indy
	{
		class ThreadPool
		{
		public:
			ThreadPool(const Thread::StartFun& threadFun, const size_t& threadCount = Thread::HardwareConcurrency(), IAtomic* sharedState = nullptr);
			~ThreadPool();

			size_t GetThreadCount();

			const std::thread::id& GetNativeThreadID(const size_t& index);
			const Thread::Status& GetThreadStatus(const size_t& index);

			void Restart();
			void RestartThread(const size_t& index);

		private:
			Thread::StartFun m_ThreadFun;
			std::vector<std::unique_ptr<Thread>> m_Threads;
			IAtomic* m_SharedState;
		};
	}
}