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

			enum Status : uint8_t { Idle = 0, Running, Stopped };

			struct State
			{
				Atomic<Status>* local;
				IAtomic* shared;
			};

			typedef std::function<void(const State&)> StartFun;

		public:
			Thread(const StartFun& startFun, IAtomic* sharedState = nullptr);
			~Thread();

			const std::thread::id& GetID();

			Status GetStatus();

		private:
			std::thread::id m_ID;
			std::thread m_Thread;

			Atomic<Status> m_Status;
			State m_State;
		};
	}
}