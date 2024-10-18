module;

#include <memory>
#include <set>

#include <future>

export module Indy.JobSystem;

export import :Impl;
export import :WorkerThread;
export import :State;
export import :Job;

export
{
	namespace Indy::JobSystem
	{
		std::unique_ptr<JobSystem_Impl> g_JobSystem_Impl = nullptr;

		void Init(const std::set<std::pair<uint32_t, size_t>>& threadMasks, const size_t& queueBufferSize);

		void Shutdown();

		void Start();

		std::shared_ptr<Job> CreateJob(const JobType& type);

		void Submit(Job& job);
		void Submit(Job&& job);
		
		void Submit(const std::shared_ptr<Job>& job);

		template<typename F, typename... Args>
		auto Submit(const JobType& type, F&& task, Args&&... taskArgs) -> std::future<decltype(task(taskArgs...))>
		{
			std::shared_ptr<Job> job = std::make_shared<Job>(type);

			auto future = job->BindTask(std::forward<F>(task), std::forward<Args>(taskArgs)...);

			g_JobSystem_Impl->Submit(job);

			return std::move(future);
		};
	}
}