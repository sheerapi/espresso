#pragma once
#include "core/jobs/JobTypes.h"
#include <atomic>

namespace core::jobs
{
	class Job
	{
	public:
		using WorkFunction = void (*)(Job*, void*);

		Job(WorkFunction work, void* data, JobPriority priority = JobPriority::Normal)
			: priority(priority), id(nextJobID.fetch_add(1)), work(work), data(data)
		{
		}

		void execute();
		auto getState() -> JobState
		{
			return state;
		}

	protected:
		std::atomic<JobState> state;
		JobPriority priority;
		JobID id;
		WorkFunction work;
        void* data;

		std::atomic<uint32_t> refCount{0}; // dependency number
        Job* parent;

	private:
		inline static std::atomic<unsigned long> nextJobID{1};

		friend class JobManager;
	};
}