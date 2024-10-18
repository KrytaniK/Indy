#include <Engine/Core/LogMacros.h>

#include <span>
#include <thread>
#include <mutex>
#include <string>
#include <sstream>

import Indy.JobSystem;

namespace Indy::JobSystem
{
	WorkerThread::WorkerThread(const uint32_t& usageMask, const WorkerThreadState& initialState)
		: m_State(initialState), m_UsageMask(usageMask)
	{
		m_State.sleep = std::make_shared<std::atomic<bool>>(true);
		m_State.sleepCV = std::make_shared<std::condition_variable>();
		m_State.sleepMutex = std::make_shared<std::mutex>();
	}

	WorkerThread::WorkerThread(const WorkerThread& other)
	{
		m_State = other.m_State;
		m_UsageMask = other.m_UsageMask;
	}

	WorkerThread::~WorkerThread()
	{
		Join();
	}

	uint32_t WorkerThread::GetUsage()
	{
		return m_UsageMask;
	}

	void WorkerThread::Start()
	{
		// If the thread is joinable, it's already been started
		if (m_Thread.joinable())
		{
			INDY_CORE_ERROR("Failed to start worker thread: Thread already started!");
			return;
		}

		// Will call operator() overload (requires copy constructor)
		// NOTICE: This will create a COPY of this object
		m_Thread = std::thread(*this);
	}

	void WorkerThread::Sleep()
	{
		m_State.sleep->store(true, std::memory_order_release);
	}

	void WorkerThread::Wake()
	{
		{
			std::lock_guard lk(*m_State.sleepMutex);
			m_State.sleep->store(false, std::memory_order_release);
		}
		m_State.sleepCV->notify_all();
	}

	void WorkerThread::Join()
	{
		if (m_Thread.joinable())
			m_Thread.join();
	}

	// NOTE: This operates on a COPY of the original object
	void WorkerThread::operator()()
	{
		// Get this thread's ID
		std::stringstream stream;
		stream << std::this_thread::get_id();
		std::string localThreadID = stream.str();

		// Get a copy of the shared pointer to this thread's local queue
		auto& localQueue = m_State.shared->threadQueues[m_State.localQueueIndex];

		// Do work until a shutdown is requested, Idle if needed
		while (!m_State.shared->shutdown.load(std::memory_order_acquire))
			OnThreadWork(localThreadID, localQueue);

		// Finish up any pending jobs, and exit
		OnThreadExit(localThreadID, localQueue);
	}

	void WorkerThread::OnThreadIdle(const std::string& threadID, std::shared_ptr<WorkerThreadQueue>& localQueue)
	{
		// Force idle state to prevent excessive use of CPU resources
		m_State.shared->idleThreadCount++; // Increment shared counter
		{
			// Lock this thread's mutex
			std::unique_lock lk(*m_State.sleepMutex);
			
			// Sleep until this thread's sleep flag is set to false.
			m_State.sleepCV->wait(lk, [this]() { return !m_State.sleep->load(std::memory_order_acquire); });
		}
		m_State.shared->idleThreadCount--; // Decrement shared counter
	}

	void WorkerThread::OnThreadWork(const std::string& threadID, std::shared_ptr<WorkerThreadQueue>& localQueue)
	{
		// Idle wait until work becomes available
		if (localQueue->handle->IsEmpty())
			OnThreadIdle(threadID, localQueue);

		// Check for work in local queue
		while (!localQueue->handle->IsEmpty())
		{
			// Attempt to pull work from the local queue
			std::shared_ptr<Job> job = nullptr;
			{
				std::unique_lock lk(*localQueue->mutex);
				job = localQueue->handle->Dequeue();
			}

			// If the job is null, something went wrong.
			// Continue for now
			if (!job)
			{
				INDY_CORE_ERROR("Bad Job");
				continue;
			}

			// Execute the job if all is good
			job->Execute();
		}

		// Attempt to sleep once all work is exhausted
		m_State.sleep->store(true, std::memory_order_release);
	}

	void WorkerThread::OnThreadExit(const std::string& threadID, std::shared_ptr<WorkerThreadQueue>& localQueue)
	{
		// Finish up any remaining work
		while (!localQueue->handle->IsEmpty())
		{
			// Attempt to pull work from the local queue
			std::shared_ptr<Job> job = nullptr;
			{
				std::unique_lock lk(*localQueue->mutex);
				job = localQueue->handle->Dequeue();
			}

			// If the job is null, something went wrong.
			// Continue for now
			if (!job)
				continue;

			// Execute the job if all is good
			job->Execute();
		}
	}

}