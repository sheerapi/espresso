#pragma once
#include "Job.h"
#include "concurrentqueue.h"
#include "core/jobs/WorkerThread.h"
#include <condition_variable>
#include <memory>
#include <vector>

namespace core::jobs
{
	struct Statistics
	{
		std::atomic<uint64_t> totalJobsExecuted;
		std::atomic<uint64_t> totalJobsSubmitted;
		std::atomic<uint64_t> averageJobsPerFrame;
		std::atomic<std::chrono::microseconds> averageJobExecutionTime;
		std::atomic<std::chrono::microseconds> averageFrameTime;
		std::atomic<size_t> currentQueueSize;
	};

	class JobManager
	{
	public:
		static void initialize();
		static void shutdown();

		static auto submitJob(const std::shared_ptr<Job>& job) -> JobID;
		
	protected:
		static auto dequeueJob() -> std::shared_ptr<Job>;

		inline static moodycamel::ConcurrentQueue<std::shared_ptr<Job>> jobQueues[5];
		inline static std::vector<std::unique_ptr<WorkerThread>> workerThreads;
		inline static Statistics stats;
        inline static std::atomic<bool> shutdownRequested{false};
		inline static std::mutex mutex;
		inline static std::condition_variable condition;
		inline static std::atomic<JobID> activeJobCount;
		inline static thread_local WorkerThread* currentWorkerThread;

		friend class WorkerThread;
	};
}