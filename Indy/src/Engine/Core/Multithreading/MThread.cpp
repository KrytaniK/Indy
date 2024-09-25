#include <Engine/Core/LogMacros.h>

#include <functional>
#include <thread>
#include <atomic>

#include <sstream>

import Indy.Multithreading;

namespace Indy
{
	Thread::Thread(const ThreadStartFun& startFun, IAtomic* sharedState)
	{
		m_Status.Store(THREAD_STATUS_RUNNING);
		m_State.status = &m_Status;
		m_State.shared = sharedState;

		m_Thread = std::thread(startFun, &m_State);
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

	const std::thread::id& Thread::GetNativeID()
	{
		return m_ID;
	}

	ThreadStatus Thread::GetStatus()
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
		m_Status.Store(THREAD_STATUS_JOINED);
	}

	void Thread::Detach()
	{
		m_Thread.detach();
		m_Status.Store(THREAD_STATUS_DETACHED);
	}

	void Thread::Restart(const ThreadStartFun& startFun, IAtomic* sharedState)
	{
		if (!IsJoinable())
			return; // Maybe do something useful here

		// Finish any work
		Join();

		// Attach potentially new shared state and usageFlags
		m_State.shared = sharedState;

		std::stringstream s;
		s << m_ID;

		INDY_CORE_INFO("Restarting Thread! Old thread [{0}] will be deleted!", s.str());

		// "Restart" the thread
		m_Status.Store(THREAD_STATUS_RUNNING);
		m_Thread = std::thread(startFun, &m_State);
		m_ID = m_Thread.get_id();

		s << m_ID;
		INDY_CORE_INFO("Thread Restarted! New Thread ID: {0}", s.str());
	}
}