module;

#include <set>
#include <vector>
#include <memory>
#include <atomic>

export module Indy.JobSystem:Impl;

import Indy.Multithreading;

import :State;
import :Job;

export {
	namespace Indy::JobSystem
	{
		class JobSystem_Impl
		{
		public:
			JobSystem_Impl(const std::set<std::pair<uint32_t, size_t>>& threadMasks, const size_t& queueBufferSize);
			~JobSystem_Impl();

			void Start();

			void Submit(Job& job);
			void Submit(Job&& job);
			void Submit(const std::shared_ptr<Job>& job);

		private:
			size_t GetLeastLoadedQueue(const uint32_t& jobType);

		private:
			JobSystemState m_State;
		};
		
	}
}