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
			inline static uint8_t s_ThreadCount = 0;

			enum Status { Idle = 0, Running, Joined, Detached };

			struct State
			{
				Atomic<Status>* status;
				IAtomic* shared;
			};

			typedef std::function<void(Atomic<State>*)> StartFun;

			static unsigned int HardwareConcurrency();

		public:
			Thread(const StartFun& startFun, IAtomic* sharedState = nullptr);
			~Thread();

			// Observation Functions

			const std::thread::id& GetID();

			Status GetStatus();

			bool IsJoinable();

			std::thread::native_handle_type GetNativeHandle();

			// Operations

			void Join();

			void Detach();

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