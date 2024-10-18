#include <Engine/Core/LogMacros.h>

#include <set>
#include <thread>
#include <vector>
#include <mutex>

import Indy.JobSystem;
import Indy.Multithreading;

namespace Indy::JobSystem
{
	JobSystem_Impl::JobSystem_Impl(const std::set<std::pair<uint32_t, size_t>>& threadMasks, const size_t& queueBufferSize)
	{
		// Parse thread masks to get the number of threads
		m_State.threadCount = 0;
		for (const auto& pair : threadMasks)
			m_State.threadCount = m_State.threadCount + pair.second;

		// Initialize shared state
		m_State.shared = std::make_unique<ThreadSharedState>();

		// Reserve enough space for the threads and their queues
		m_State.threads.reserve(m_State.threadCount);
		m_State.shared->threadQueues.resize(m_State.threadCount);
		
		// Create all thread queues with thier mutexes
		for (size_t i = 0; i < m_State.threadCount; i++)
		{
			auto& queue = m_State.shared->threadQueues[i];
			queue = std::make_shared<WorkerThreadQueue>();
			queue->handle = std::make_unique<RingQueue<Job>>(queueBufferSize);
			queue->mutex = std::make_unique<std::mutex>();
		}

		// Create all threads, with assigned local state
		WorkerThreadState threadState{};
		threadState.shared = m_State.shared;

		size_t threadQueueIndex = 0;
		for (const auto& pair : threadMasks)
		{
			for (size_t i = 0; i < pair.second; i++)
			{
				threadState.localQueueIndex = threadQueueIndex;
				m_State.threads.push_back(std::make_unique<WorkerThread>(pair.first, threadState));
				m_State.threads.back()->Start();
				threadQueueIndex++;
			}
		}

		// Wait until all threads have finished initialization
		while (m_State.shared->idleThreadCount.load(std::memory_order_acquire) != m_State.threadCount)
		{
			// Busy wait until all threads have been initialized
		}

		INDY_CORE_INFO("[Job System] Finished Initializing with {0} threads.", m_State.threadCount);
	}

	JobSystem_Impl::~JobSystem_Impl()
	{
		INDY_CORE_INFO("[Job System] Shutting Down...");
		m_State.shared->shutdown.store(true, std::memory_order_release);

		// Finish any in-process work before shutting down
		for (const auto& thread : m_State.threads)
		{
			thread->Wake();
			thread->Join();
		}
	}

	void JobSystem_Impl::Start()
	{
		// Wake up all threads
		for (const auto& thread : m_State.threads)
			thread->Wake();
	}

	void JobSystem_Impl::Submit(Job& job)
	{
		// Get the index of a queue with a matching type description
		size_t index = GetLeastLoadedQueue(job.GetType());

		if (index == SIZE_MAX)
		{
			INDY_CORE_ERROR("Failed to submit job: No threads support job type [{0}]", static_cast<uint32_t>(job.GetType()));
			return;
		}

		// Pull a reference to the queue
		auto& queue = m_State.shared->threadQueues[index];

		// Lock the queue's handle via its mutex and
		//	enqueue the job
		{
			std::unique_lock lk(*queue->mutex);
			queue->handle->Enqueue(job);
		}

		// Wake the relevant thread
		m_State.threads[index]->Wake();
	}

	void JobSystem_Impl::Submit(Job&& job)
	{
		// Get the index of a queue with a matching type description
		size_t index = GetLeastLoadedQueue(job.GetType());

		if (index == SIZE_MAX)
		{
			INDY_CORE_ERROR("Failed to submit job: No threads support job type [{0}]", static_cast<uint32_t>(job.GetType()));
			return;
		}

		// Pull a reference to the queue
		auto& queue = m_State.shared->threadQueues[index];

		// Lock the queue's handle via its mutex and
		//	enqueue the job
		{
			std::unique_lock lk(*queue->mutex);
			queue->handle->Enqueue(std::move(job));
		}

		// Wake the relevant thread
		m_State.threads[index]->Wake();
	}

	void JobSystem_Impl::Submit(const std::shared_ptr<Job>& job)
	{
		// Get the index of a queue with a matching type description
		size_t index = GetLeastLoadedQueue(job->GetType());

		if (index == SIZE_MAX)
		{
			INDY_CORE_ERROR("Failed to submit job: No threads support job type [{0}]", static_cast<uint32_t>(job->GetType()));
			return;
		}

		// Pull a reference to the queue
		auto& queue = m_State.shared->threadQueues[index];

		// Lock the queue's handle via its mutex and
		//	enqueue the job
		{
			std::unique_lock lk(*queue->mutex);
			queue->handle->Enqueue(std::move(job));
		}

		// Wake the relevant thread
		m_State.threads[index]->Wake();
	}

	size_t JobSystem_Impl::GetLeastLoadedQueue(const uint32_t& jobType)
	{
		size_t leastLoaded = SIZE_MAX;
		for (size_t i = 0; i < m_State.threadCount; i++)
		{
			// For each thread, compare the thread's usage flags
			//	against the job type. If the thread has that job's
			//	flag(s) set in its usage flags, it can take the job
			uint32_t threadUsage = m_State.threads[i]->GetUsage();
			if (jobType <= threadUsage)
			{
				// If the current known index is invalid, then this queue
				//	is under the lowest load
				if (leastLoaded == SIZE_MAX)
				{
					leastLoaded = i;
					continue;
				}

				// Otherwise, if this queue is under less load than the current queue,
				//	then this queue becomes the queue with the lowest load
				auto& compQueue = m_State.shared->threadQueues[i]->handle;
				size_t currentLoad = m_State.shared->threadQueues[leastLoaded]->handle->Size();
				if (currentLoad > compQueue->Size())
				{
					leastLoaded = i;
				}
			}
		}

		return leastLoaded;
	}
}