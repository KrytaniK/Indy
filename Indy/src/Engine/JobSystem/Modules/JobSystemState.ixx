module;

#include <vector>
#include <span>

#include <mutex>
#include <condition_variable>
#include <atomic>

export module Indy.JobSystem:State;

import :Job;

import Indy.Multithreading;
import Indy.Utility;

export
{
	namespace Indy::JobSystem
	{
		struct WorkerThreadQueue
		{
			std::unique_ptr<RingQueue<Job>> handle;
			std::unique_ptr<std::mutex> mutex;
		};

		struct ThreadSharedState
		{
			std::vector<std::shared_ptr<WorkerThreadQueue>> threadQueues;
			std::atomic<bool> shutdown{false};
			std::atomic<size_t> idleThreadCount{0};
		};

		struct JobSystemState
		{
			std::vector<std::unique_ptr<IThread>> threads;
			std::shared_ptr<ThreadSharedState> shared;
			size_t threadCount = 0;
		};

		struct WorkerThreadState
		{
			// Shared State
			std::shared_ptr<ThreadSharedState> shared = nullptr;

			// Local State
			size_t localQueueIndex = 0;

			// Conditional Waking
			std::shared_ptr<std::atomic<bool>> sleep;
			std::shared_ptr<std::condition_variable> sleepCV;
			std::shared_ptr<std::mutex> sleepMutex;
		};
	}
}