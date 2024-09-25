module;

#include <Engine/Core/LogMacros.h>

#include <cstdint>
#include <functional>
#include <thread>
#include <atomic>

export module Indy.Multithreading:Thread;

import :Atomic;

export
{
	namespace Indy
	{
		typedef enum ThreadStatus
		{
			THREAD_STATUS_IDLE = 0,
			THREAD_STATUS_RUNNING = 1,
			THREAD_STATUS_JOINED = 2,
			THREAD_STATUS_DETACHED = 3,
			THREAD_STATUS_MAX_ENUM = 4,
		} ThreadStatus;

		struct ThreadState
		{
			Atomic<ThreadStatus>* status = nullptr;
			IAtomic* shared = nullptr;
		};

		typedef std::function<void(ThreadState*)> ThreadStartFun;

		unsigned int HardwareConcurrency() { return std::thread::hardware_concurrency(); };

		class IThread
		{
		public:
			virtual ~IThread() = default;

			virtual const std::thread::id& GetNativeID() = 0;
			virtual std::thread::native_handle_type GetNativeHandle() = 0;

			virtual ThreadStatus GetStatus() = 0;

			virtual bool IsJoinable() = 0;

			virtual void Join() = 0;
			virtual void Detach() = 0;
		};

		class Thread : public IThread
		{
		public:
			inline static unsigned int s_ThreadCount = 0;

		public:
			Thread(const ThreadStartFun& startFun, IAtomic* sharedState = nullptr);
			virtual ~Thread() override;

			virtual const std::thread::id& GetNativeID() override;
			virtual std::thread::native_handle_type GetNativeHandle() override;

			virtual ThreadStatus GetStatus() override;

			virtual bool IsJoinable() override;

			virtual void Join() override;
			virtual void Detach() override;

			void Restart(const ThreadStartFun& startFun, IAtomic* sharedState);

		private:
			std::thread::id m_ID;
			std::thread m_Thread;

			Atomic<ThreadStatus> m_Status;
			ThreadState m_State;
		};
	}
}