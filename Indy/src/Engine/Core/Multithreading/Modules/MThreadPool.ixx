module;

#include <Engine/Core/LogMacros.h>

#include <cstdint>
#include <vector>
#include <set>
#include <thread>
#include <memory>
#include <stdexcept>

export module Indy.Multithreading:ThreadPool;

import :Atomic;
import :Thread;

export
{
	namespace Indy
	{
		template<typename T>
		class ThreadPool
		{
			static_assert(std::is_convertible<T*, IThread*>::value, "Thread pool template argument is not derived from base type <IThread>!");

		public:
			template<typename... Args>
			ThreadPool(const size_t& threadCount = HardwareConcurrency(), Args&&... args);
			~ThreadPool();

			size_t GetThreadCount();

			std::weak_ptr<T> GetThread(const size_t& index);

			template<typename... Args>
			void Restart(Args... args);

		private:
			ThreadStartFun m_ThreadFun;
			std::vector<std::shared_ptr<T>> m_Threads;
			IAtomic* m_SharedState;
		};

		template<typename T>
		template<typename ...Args>
		ThreadPool<T>::ThreadPool(const size_t& threadCount, Args&&... args)
		{
			m_Threads.reserve(threadCount);

			for (size_t i = 0; i < threadCount; i++)
				m_Threads.emplace_back(std::make_shared<T>(args...));

			INDY_CORE_INFO("Thread Pool Initialized with {0} threads!", threadCount);
		}

		template<typename T>
		ThreadPool<T>::~ThreadPool()
		{
			// Threads automatically join on destruction.
			// Shared thread state is not managed by this class.
		}

		template<typename T>
		size_t ThreadPool<T>::GetThreadCount()
		{
			return m_Threads.size();
		}

		template<typename T>
		std::weak_ptr<T> ThreadPool<T>::GetThread(const size_t& index)
		{
			if (index > m_Threads.size() - 1)
				throw std::runtime_error("Thread index out of bounds!");

			return m_Threads[index];
		}

		template<typename T>
		template<typename ...Args>
		void ThreadPool<T>::Restart(Args... args)
		{
			m_Threads.clear();

			for (size_t i = 0; i < m_Threads.size(); i++)
				m_Threads.emplace_back(std::make_shared<T>(args...));
		}
	}
}