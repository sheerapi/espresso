#include "core/jobs/WorkerThread.h"
#include "core/Application.h"
#include "core/jobs/JobManager.h"
#include "core/jobs/JobTypes.h"
#include <chrono>

namespace core::jobs
{
	WorkerThread::WorkerThread(ThreadID id)
	{
		this->id = id;
	}

	void WorkerThread::start()
	{
		thread = std::make_unique<std::thread>(&WorkerThread::workerThreadMain, this);
	}

	WorkerThread::~WorkerThread()
	{
		stop();
	}

	void WorkerThread::stop()
	{
		requestStop();

		if (thread && thread->joinable())
		{
			thread->join();
		}
	}

	auto WorkerThread::executeNextJob() -> bool
	{
		auto job = JobManager::dequeueJob();

		if (job)
		{
			executeJob(job);
			return job->getState() == JobState::Completed;
		}

        return false;
	}

	void WorkerThread::executeJob(const std::shared_ptr<Job>& job)
	{
		auto start = std::chrono::high_resolution_clock::now();
		job->execute();
		jobsExecuted.fetch_add(1);

		auto end = std::chrono::high_resolution_clock::now();

		totalExecutionTime =
			totalExecutionTime.load() +
			std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	}

	void WorkerThread::workerThreadMain()
	{
        // set affinity
		cpu_set_t cpuset;
		CPU_ZERO(&cpuset);
		CPU_SET(id % std::thread::hardware_concurrency(), &cpuset);
		pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

        // set thread priority
		int policy;
		struct sched_param param;

		pthread_getschedparam(pthread_self(), &policy, &param);
		param.sched_priority = sched_get_priority_max(policy);
		pthread_setschedparam(pthread_self(), policy, &param);

		JobManager::currentWorkerThread = this;

		while (!shouldStop)
		{
			std::unique_lock<std::mutex> lock(mutex);
			JobManager::condition.wait(
				lock, [=]() { return JobManager::activeJobCount != 0 || JobManager::shutdownRequested; });

			executeNextJob();
		}

		JobManager::currentWorkerThread = nullptr;
	}

	void WorkerThread::yield()
	{
		executeNextJob();
	}
}