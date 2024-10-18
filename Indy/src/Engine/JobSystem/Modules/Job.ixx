module;

#include <Engine/Core/LogMacros.h>

#include <cstdint>
#include <memory>
#include <functional>
#include <future>
#include <atomic>

export module Indy.JobSystem:Job;

export 
{
	namespace Indy::JobSystem
	{
		typedef enum JobType : uint32_t
		{
			JOB_TYPE_GENERAL = 0x00,
			JOB_TYPE_RESOURCE_LOAD = 0x01,
			JOB_TYPE_GPU_RESOURCE = 0x04,
			JOB_TYPE_MAX_ENUM = UINT32_MAX
		} JobType;

		class IJob
		{
		public:
			virtual ~IJob() = default;

			virtual uint32_t GetType() const = 0;

			virtual void Execute() = 0;
		};

		class Job : public IJob
		{
		public:
			Job(const uint32_t& type = 0) : m_Type(type) { };
			virtual ~Job() final = default;

			virtual uint32_t GetType() const final 
			{ 
				return m_Type; 
			};
			
			virtual void Execute() final 
			{
				// Execute this job, only if another thread hasn't done so
				if (m_OnExecute)
				{
					m_OnExecute();
				}
			};

			template<typename F, typename... Args>
			auto BindTask(F&& func, Args&&... args) -> std::future<decltype(func(args...))>
			{
				// Create a wrapper function, with bound parameters
				auto f = std::bind(std::forward<F>(func), std::forward<Args>(args)...);

				// Encapsulate the packaged_task in a shared_ptr to allow copying
				auto packaged_ptr = std::make_shared<std::packaged_task<decltype(func(args...))(Args&&...)>>(f);

				// Set internal onExecute function
				m_OnExecute = [packaged_ptr]() 
					{ 
						(*packaged_ptr)();
					};

				return packaged_ptr->get_future();
			}

			Job(const Job& other) = delete;

			Job(Job&& other) 
			{ 
				m_Type = std::move(other.m_Type); 
				m_OnExecute = std::move(other.m_OnExecute);
			};

		private:
			uint32_t m_Type;
			std::function<void()> m_OnExecute;
		};
	}
}