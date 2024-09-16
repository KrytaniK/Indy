#include <Engine/Core/LogMacros.h>

#include <functional>
#include <thread>
#include <atomic>

#include <sstream>

import Indy.Multithreading;

namespace Indy
{
	unsigned int Thread::HardwareConcurrency()
	{
		return std::thread::hardware_concurrency();
	}

	Thread::Thread(const Thread::StartFun& startFun, IAtomic* sharedState)
	{
		m_Status.Store(Status::Running);
		m_State.status = &m_Status;
		m_State.shared = sharedState;

		m_Thread = std::thread(startFun, m_State);
		m_ID = m_Thread.get_id();

		std::stringstream s;
		s << m_ID;

		s_ThreadCount++;
		INDY_CORE_INFO("Thread Created With ID: {0}", s.str());
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
		m_Status.Store(Thread::Joined);
	}

	void Thread::Detach()
	{
		m_Thread.detach();
		m_Status.Store(Thread::Detached);
	}

	void Thread::Restart(const Thread::StartFun& startFun, IAtomic* sharedState)
	{
		if (!IsJoinable())
			return; // Maybe do something useful here

		// Finish any work
		Join();

		// Attach potentially new shared state
		m_State.shared = sharedState;

		// "Restart" the thread
		m_Status.Store(Thread::Running);
		m_Thread = std::thread(startFun, m_State);
		m_ID = m_Thread.get_id();
	}
}