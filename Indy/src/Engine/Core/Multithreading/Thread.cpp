#include <Engine/Core/LogMacros.h>

#include <functional>
#include <thread>
#include <atomic>

import Indy.Multithreading;

namespace Indy
{
	unsigned int Thread::HardwareConcurrency()
	{
		return std::thread::hardware_concurrency();
	}

	Thread::Thread(const Thread::StartFun& startFun, IAtomic* sharedState)
	{
		// Attach shared state
		if (sharedState != nullptr)
			m_State.shared = sharedState;

		// Attach local state
		m_State.local = &m_Status;

		// Ensure we aren't creating more threads than are available.
		const int maxThreadCount = HardwareConcurrency();
		if (s_ThreadCount + 1 == maxThreadCount)
		{
			INDY_CORE_WARN("The number of threads has reached the hardware limit of {0}. Creating more than this may result in performance issues!", maxThreadCount);
		}

		// Create the new thread with the start function
		m_Thread = std::thread(startFun, m_State);
		m_ID = m_Thread.get_id();

		// Increase Global Thread Count
		s_ThreadCount++;
	}

	Thread::~Thread()
	{
		if (IsJoinable())
			m_Thread.join();

		s_ThreadCount--;
	}

	const std::thread::id& Thread::GetID()
	{
		return m_ID;
	}

	Thread::Status Thread::GetStatus()
	{
		return m_Status.Load();
	}

	bool Thread::IsJoinable()
	{
		return m_Thread.joinable();
	}

	std::thread::native_handle_type Thread::GetNativeHandle()
	{
		return m_Thread.native_handle();
	}

	void Thread::Join()
	{
		if (!IsJoinable()) 
			return;

		m_Thread.join();
		m_State.local->Store(Thread::Joined);
	}

	void Thread::Detach()
	{
		m_Thread.detach();
		m_State.local->Store(Thread::Detached);
	}

	void Thread::Swap(Thread& other)
	{
		// Execute thread handle swap
		std::swap(m_Thread, other.m_Thread);

		// Swap Thread Status and State(local and shared)
		Thread::Status tempStatus = m_Status.Load();
		m_Status.Store(other.m_Status.Load());
		other.m_Status.Store(tempStatus);

		IAtomic* tempAtomic = m_State.shared;
		m_State.shared = other.m_State.shared;
		other.m_State.shared = tempAtomic;

		// Swap Thread ID (with accurate thread id)
		m_ID = m_Thread.get_id();
		other.m_ID = other.m_Thread.get_id();
	}

}