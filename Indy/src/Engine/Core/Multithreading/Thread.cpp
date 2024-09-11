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
		// Ensure we aren't creating more threads than are available.
		const int maxThreadCount = HardwareConcurrency();
		if (s_ThreadCount + 1 == maxThreadCount)
		{
			INDY_CORE_WARN("The number of threads has reached the hardware limit of {0}. Creating more than this may result in performance issues!", maxThreadCount);
		}

		m_Status.Store(Status::Running);
		m_State.status = &m_Status;

		Thread::State initialState;

		m_Thread = std::thread(startFun, &m_State);
		m_ID = m_Thread.get_id();

		// Increase Global Thread Count
		s_ThreadCount++;
	}

	Thread::~Thread()
	{
		Join();
		s_ThreadCount--;
	}

	const std::thread::id& Thread::GetID()
	{
		return m_ID;
	}

	Thread::Status Thread::GetStatus()
	{
		return m_Status->Load();
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
		m_Status.Load(Thread::Joined);
	}

	void Thread::Detach()
	{
		m_Thread.detach();
		m_Status.Load(Thread::Detached);
	}
}