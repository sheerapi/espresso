#pragma once
#include "core/jobs/Job.h"
#include "core/jobs/JobTypes.h"
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>

namespace core::jobs
{
    class WorkerThread
    {
    public:
        explicit WorkerThread(ThreadID id);
        ~WorkerThread();

        void start();
        void stop();

		void requestStop()
		{
			shouldStop.store(true);
		}

		auto executeNextJob() -> bool;
		void executeJob(const std::shared_ptr<Job>& job);

	protected:
        void workerThreadMain();

        ThreadID id;
        std::unique_ptr<std::thread> thread;
		std::atomic<bool> shouldStop{false};
        std::mutex mutex;

		std::atomic<uint64_t> jobsExecuted{0};
		std::atomic<std::chrono::microseconds> totalExecutionTime{
			std::chrono::microseconds::zero()};
	};
}