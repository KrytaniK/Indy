#include <Engine/Core/LogMacros.h>

#include <cstdint>
#include <vector>
#include <thread>

import Indy.Multithreading;

namespace Indy
{
	ThreadPool::ThreadPool(const Thread::StartFun& threadFun, const size_t& threadCount, IAtomic* sharedState)
	{
		m_Threads.reserve(threadCount);
		for (size_t i = 0; i < threadCount; i++)
			m_Threads.emplace_back(std::make_unique<Thread>(threadFun, sharedState));

		INDY_CORE_INFO("Thread Pool Initialized with {0} threads!", threadCount);
	}

	ThreadPool::~ThreadPool()
	{
		// Threads automatically join on destruction.
		// Shared thread state is not managed by this class.
	}

	size_t ThreadPool::GetThreadCount()
	{
		return m_Threads.size();
	}

	const std::thread::id& ThreadPool::GetNativeThreadID(const size_t& index)
	{
		if (index >= m_Threads.size())
		{
			INDY_CORE_ERROR("Failed to get native thread id for index {0}: Invalid index extends beyond thread count of {1}!", index, m_Threads.size());
			return std::thread::id();
		}

		return m_Threads[index]->GetID();
	}

	const Thread::Status& ThreadPool::GetThreadStatus(const size_t& index)
	{
		if (index >= m_Threads.size())
		{
			INDY_CORE_ERROR("Failed to get thread status for index {0}: Invalid index extends beyond thread count of {1}!", index, m_Threads.size());
			return Thread::Max_Enum;
		}

		return m_Threads[index]->GetStatus();
	}

	void ThreadPool::Restart()
	{
		for (size_t i = 0; i < m_Threads.size(); i++)
			m_Threads[i]->Restart(m_ThreadFun, m_SharedState);
	}

	void ThreadPool::RestartThread(const size_t& index)
	{
		if (index >= m_Threads.size())
		{
			INDY_CORE_ERROR("Failed to restart thread in thread pool: Invalid Index.");
			return;
		}

		m_Threads[index]->Restart(m_ThreadFun, m_SharedState);
	}
}