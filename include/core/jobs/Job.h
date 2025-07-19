#pragma once
#include "JobTypes.h"
#include "JobMemoryPool.h"
#include <functional>
#include <atomic>
#include <memory>

namespace core::jobs
{
	class Job
	{
	public:
		using WorkFunction = std::function<void(Job*)>;

		Job(WorkFunction work, JobPriority priority = JobPriority::Normal);
		virtual ~Job() = default;

		void execute();

		void addDependency(JobID jobId);
		void addDependency(std::shared_ptr<Job> job);
		[[nodiscard]] auto areDependenciesSatisfied() const -> bool;

		auto createChildJob(WorkFunction work,
							 JobPriority priority = JobPriority::Normal) -> JobID;
		void waitForChildren();

		void waitForDependencies();

		[[nodiscard]] auto getId() const -> JobID
		{
			return _id;
		}

		[[nodiscard]] auto getPriority() const -> JobPriority
		{
			return _priority;
		}

		[[nodiscard]] auto getState() const -> JobState
		{
			return _state.load();
		}

		[[nodiscard]] auto getExecutingThread() const -> ThreadID
		{
			return _executingThread;
		}

		template <typename T> auto allocate(size_t count = 1) -> T*;

	protected:
		virtual void onExecute()
		{
		}

	private:
		static std::atomic<JobID> nextJobId;

		JobID _id;
		WorkFunction _work;
		JobPriority _priority;
		std::atomic<JobState> _state;
		ThreadID _executingThread;

		std::vector<JobID> _dependencies;
		std::atomic<uint32_t> _pendingDependencies;

		std::vector<JobID> _childJobs;
		std::atomic<uint32_t> _pendingChildren;
		JobID _parentJob;

        std::unique_ptr<JobMemoryPool> _memoryPool;
	};
}