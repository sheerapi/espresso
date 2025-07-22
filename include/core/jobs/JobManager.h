#pragma once
#include "Job.h"
#include "concurrentqueue.h"
#include "core/jobs/WorkerThread.h"
#include <condition_variable>
#include <memory>
#include <unordered_map>
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
        static auto getJob(JobID job) -> std::shared_ptr<Job>;
		static void addDependency(const std::shared_ptr<Job>& dependent, const std::shared_ptr<Job>& dependency);

	protected:
		static auto dequeueJob() -> std::shared_ptr<Job>;
		static void onComplete(JobID id);

		inline static moodycamel::ConcurrentQueue<std::shared_ptr<Job>> jobQueues[5];
		inline static std::vector<std::unique_ptr<WorkerThread>> workerThreads;
		inline static Statistics stats;
        inline static std::atomic<bool> shutdownRequested{false};
		inline static std::mutex mutex;
		inline static std::condition_variable condition;
		inline static std::atomic<JobID> activeJobCount;
		inline static thread_local WorkerThread* currentWorkerThread;
		inline static std::unordered_map<JobID, std::vector<std::shared_ptr<Job>>>
			dependents;

		friend class WorkerThread;
        friend class Job;
	};
}