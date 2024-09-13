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
		class Thread
		{
		public:
			inline static int s_ThreadCount = 0;

			enum Status { Idle = 0, Running, Joined, Detached };

			struct State
			{
				Atomic<Status>* status;
				IAtomic* shared;
			};

			typedef std::function<void(const State&)> StartFun;

			static unsigned int HardwareConcurrency();

		public:
			Thread(const StartFun& startFun, IAtomic* sharedState = nullptr);
			~Thread();

			const std::thread::id& GetID();

			Status GetStatus();

			bool IsJoinable();

			std::thread::native_handle_type GetNativeHandle();

			void Join();
			void Detach();

			void Restart(const StartFun& startFun, IAtomic* sharedState);

		private:
			Thread(const Thread&) = delete; // No copies
			Thread& operator=(const Thread&) = delete; // No explicit assignments

		private:
			std::thread::id m_ID;
			std::thread m_Thread;

			Atomic<Status> m_Status;
			State m_State;
		};
	}
}