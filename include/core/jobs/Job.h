#pragma once
#include "core/jobs/JobTypes.h"
#include <atomic>
#include <chrono>

namespace core::jobs
{
	class Job
	{
	public:
		using WorkFunction = void (*)(Job*, void*);

		Job(WorkFunction work, void* data = nullptr, JobPriority priority = JobPriority::Normal)
			: priority(priority), id(nextJobID.fetch_add(1)), work(work), data(data)
		{
		}

		void execute();
		void waitForDependencies();

		void wait();
		auto wait(std::chrono::milliseconds timeout) -> bool;

		[[nodiscard]] auto getState() const -> JobState
		{
			return state;
		}

		[[nodiscard]] auto getPriority() const -> JobPriority
		{
			return priority;
		}

		[[nodiscard]] auto getId() const -> JobID
		{
			return id;
		}

		void setData(void* data)
		{
			this->data = data;
		}

	protected:
		std::atomic<JobState> state;
		JobPriority priority;
		JobID id;
		WorkFunction work;
        void* data;

		std::atomic<uint32_t> refCount{0}; // dependency number

	private:
		inline static std::atomic<unsigned long> nextJobID{1};

		friend class JobManager;
	};
}