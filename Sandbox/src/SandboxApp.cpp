#include <Engine/Core/LogMacros.h>

#include <memory>
#include <set>
#include <vector>
#include <span>

#include <thread>

import Sandbox;

import Indy.Graphics;
import Indy.JobSystem;
import Indy.Multithreading;
import Indy.Utility;

namespace Indy
{
	std::unique_ptr<Indy::Application> CreateApplication()
	{
		Indy::ApplicationCreateInfo createInfo;
		createInfo.name = "Sandbox App";

		return std::make_unique<Indy::Sandbox>(createInfo);
	};

	Sandbox::Sandbox(const ApplicationCreateInfo& createInfo)
		: Application(createInfo)
	{
		m_ShouldClose = true;
	}

	Sandbox::~Sandbox()
	{

	}

	void Sandbox::Load()
	{
		Graphics::Init(Graphics::Driver::Vulkan);

		std::set<std::pair<uint32_t, size_t>> masks = {
			{JobSystem::JOB_TYPE_GENERAL, 3},
			{JobSystem::JOB_TYPE_RESOURCE_LOAD, 3},
			{JobSystem::JOB_TYPE_GPU_RESOURCE, 3},
		};

		JobSystem::Init(masks, 32);
	}

	void Sandbox::Start()
	{
		JobSystem::Start();

		// Run 100,000 iterations to test the integrity of the job system
		for (size_t i = 0; i < 100000; i++)
		{
			JobSystem::Job copyJob(JobSystem::JOB_TYPE_GENERAL);
			JobSystem::Job moveJob(JobSystem::JOB_TYPE_RESOURCE_LOAD);
			std::shared_ptr<JobSystem::Job> ptrJob = std::make_shared<JobSystem::Job>(JobSystem::JOB_TYPE_GPU_RESOURCE);

			std::atomic<int> x;
			x.store(0, std::memory_order_release);

			// Bind a task to a job
			auto future1 = copyJob.BindTask([&]() { x++; });
			auto future2 = moveJob.BindTask([&]() { x++; });
			auto future3 = ptrJob->BindTask([&]() { x++; });

			// Auto-Submit jobs
			auto future4 = JobSystem::Submit(JobSystem::JOB_TYPE_GENERAL, [&]() { x++; });
			auto future5 = JobSystem::Submit(JobSystem::JOB_TYPE_RESOURCE_LOAD, [&]() { x++; });
			auto future6 = JobSystem::Submit(JobSystem::JOB_TYPE_GPU_RESOURCE, [&]() { x++; });

			// Manually submit jobs
			JobSystem::Submit(copyJob);
			JobSystem::Submit(std::move(moveJob));
			JobSystem::Submit(ptrJob);

			future1.wait();
			future2.wait();
			future3.wait();
			future4.wait();
			future5.wait();
			future6.wait();

			int value = x.load(std::memory_order_acquire);
			assert(value == 6);
		}
	}

	void Sandbox::Update()
	{

	}

	void Sandbox::Shutdown()
	{
		JobSystem::Shutdown();

		Graphics::Shutdown();
	}

	void Sandbox::Unload()
	{

	}
}