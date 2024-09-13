#include <Engine/Core/LogMacros.h>

#include <cstdint>
#include <vector>
#include <thread>

import Indy.Multithreading;

namespace Indy
{
	ThreadPool::ThreadPool(const Thread::StartFun& threadFun, const uint8_t& threadCount, IAtomic* sharedState)
	{
		size_t allocationSize = static_cast<size_t>(threadCount);

		m_Threads.reserve(allocationSize);
		for (size_t i = 0; i < allocationSize; i++)
			m_Threads.push_back(std::make_unique<Thread>(threadFun, sharedState));

		INDY_CORE_INFO("Thread Pool Initialized with {0} threads!", threadCount);
	}

	ThreadPool::~ThreadPool()
	{
		// Threads automatically join on destruction.
		// Shared thread state is not managed by this class.
	}

	const uint8_t& ThreadPool::GetThreadCount()
	{
		return static_cast<uint8_t>(m_Threads.size());
	}

	const std::thread::id& ThreadPool::GetNativeThreadID(const uint8_t& index)
	{
		return m_Threads[index]->GetID();
	}

	const Thread::Status& ThreadPool::GetThreadStatus(const uint8_t& index)
	{
		return m_Threads[index]->GetStatus();
	}

	void ThreadPool::Restart()
	{
		for (size_t i = 0; i < m_Threads.size(); i++)
			m_Threads[i]->Restart(m_ThreadFun, m_SharedState);
	}

	void ThreadPool::RestartThread(const uint8_t& index)
	{
		if (static_cast<size_t>(index) >= m_Threads.size())
		{
			INDY_CORE_ERROR("Failed to restart thread in thread pool: Invalid Index.");
			return;
		}

		m_Threads[index]->Restart(m_ThreadFun, m_SharedState);
	}
}