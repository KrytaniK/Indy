#include <Engine/Core/LogMacros.h>

#include <set>

import Indy.JobSystem;

namespace Indy::JobSystem
{
	void Init(const std::set<std::pair<uint32_t, size_t>>& threadMasks, const size_t& queueBufferSize)
	{
		g_JobSystem_Impl = std::make_unique<JobSystem_Impl>(threadMasks, queueBufferSize);
	}

	void Shutdown()
	{
		g_JobSystem_Impl = nullptr;
	}

	void Start()
	{
		g_JobSystem_Impl->Start();
	}

	std::shared_ptr<Job> CreateJob(const JobType& type)
	{
		return std::make_shared<Job>(type);
	}

	void Submit(Job& job)
	{
		g_JobSystem_Impl->Submit(job);
	}

	void Submit(Job&& job)
	{
		g_JobSystem_Impl->Submit(std::move(job));
	}

	void Submit(const std::shared_ptr<Job>& job)
	{
		g_JobSystem_Impl->Submit(job);
	}
}