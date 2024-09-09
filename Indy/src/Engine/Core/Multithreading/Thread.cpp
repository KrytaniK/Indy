#include <Engine/Core/LogMacros.h>

#include <functional>
#include <thread>
#include <atomic>

import Indy.Multithreading;

namespace Indy
{
	Thread::Thread(const Thread::StartFun& startFun, IAtomic* sharedState)
	{
		// Attach shared state
		if (sharedState != nullptr)
			m_State.shared = sharedState;

		// Ensure we aren't creating more threads than are available.
		const int maxThreadCount = std::thread::hardware_concurrency();
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
}