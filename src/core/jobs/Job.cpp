#include "core/jobs/Job.h"
#include "core/jobs/JobManager.h"
#include "core/jobs/JobTypes.h"
#include "core/log.h"

namespace core::jobs
{
	void Job::execute()
	{
		auto expectedState = JobState::Waiting;
		if (!state.compare_exchange_strong(expectedState, JobState::Running))
		{
			return; // job already running or completed
		}

		try
		{
			if (work != nullptr)
			{
				work(this, data);
			}

			state.store(JobState::Completed, std::memory_order_release);
		}
		catch (...)
		{
			state.store(JobState::Failed, std::memory_order_release);
			log_error("failed to execute job %d", id);
		}

        JobManager::onComplete(id);
	}

    void Job::waitForDependencies()
    {
        while (refCount != 0)
        {
            if (auto* worker = JobManager::currentWorkerThread)
            {
                worker->yield();
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }

	void Job::wait()
    {
        // failure is still completion
        while (state != JobState::Completed || state != JobState::Failed)
        {
			if (auto* worker = JobManager::currentWorkerThread)
			{
				worker->yield();
			}
			else
			{
				std::this_thread::yield();
			}
		}
    }

	auto Job::wait(std::chrono::milliseconds timeout) -> bool
    {
		auto startTime = std::chrono::steady_clock::now();

		while (state != JobState::Completed || state != JobState::Failed)
		{
			if (std::chrono::steady_clock::now() - startTime > timeout)
			{
				return false;
			}

			if (auto* worker = JobManager::currentWorkerThread)
			{
				worker->yield();
			}
			else
			{
				std::this_thread::yield();
			}
		}

        return true;
	}
}