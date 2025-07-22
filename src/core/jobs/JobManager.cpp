#include "core/jobs/JobManager.h"
#include "core/jobs/JobTypes.h"
#include "core/log.h"
#include <thread>

namespace core::jobs
{
	void JobManager::initialize()
	{
		// reserve 1 for main
		int numThreads = (int)std::thread::hardware_concurrency() - 1;

		workerThreads.reserve(numThreads);

		for (size_t i = 0; i < numThreads; i++)
		{
			auto thread = std::make_unique<WorkerThread>(i + 1);
			thread->start();
			workerThreads.push_back(std::move(thread));
		}

		log_info("initialized job manager with %d worker threads", numThreads);
	}

	auto JobManager::submitJob(const std::shared_ptr<Job>& job) -> JobID
	{
        if (shutdownRequested)
        {
            log_warn("sorry, we're closing! (tried to submit job while shutting down)");
            return InvalidJobID;
        }

		stats.totalJobsSubmitted++;
		stats.currentQueueSize++;
		jobQueues[(size_t)job->priority].enqueue(job);
        job->state = JobState::Waiting;
		activeJobCount.fetch_add(1);
		condition.notify_all();
		return job->id;
	}

	auto JobManager::dequeueJob() -> std::shared_ptr<Job>
	{
        // higher priority first by default (i think)
        for (auto& queue : jobQueues)
        {
            // empty, move on
            if (queue.size_approx() == 0)
            {
                continue;
            }

			std::shared_ptr<Job> job;
            if (!queue.try_dequeue(job))
            {
                continue;
            }

            // job is ready to be executed
            if (job->refCount == 0)
            {
				activeJobCount.fetch_sub(1);
                condition.notify_all();
                return job;
            }

            // its not
            queue.enqueue(job);
		}

        return nullptr;
	}

	void JobManager::shutdown()
	{
        shutdownRequested = true;
		condition.notify_all();

		for (auto& thread : workerThreads)
		{
			thread->stop();
		}

		workerThreads.clear();
	}
}