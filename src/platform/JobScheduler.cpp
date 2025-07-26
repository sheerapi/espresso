#include "platform/JobScheduler.h"
#include <thread>
#include "core/Application.h"
#include "core/log.h"

namespace internals
{
    void JobScheduler::init()
    {
        corePoolSize = (int)std::thread::hardware_concurrency() / 2;
        maxPoolSize = (int)std::thread::hardware_concurrency() * 2;
    }

	void JobScheduler::submit(const std::function<void()>& task)
	{
		bool createCoreThread = false;
		bool createNonCoreThread = false;
		{
			std::unique_lock lock(mutex);
			if (isShuttingDown)
			{
				log_error("sorry, we're closing! (job scheduler shutting down)");
                return;
			}
			taskQueue.push(task);

			if (threadCount < corePoolSize)
			{
				threadCount++;
				createCoreThread = true;
			}
			else if (threadCount < maxPoolSize && taskQueue.size() > queueThreshold)
			{
				threadCount++;
				createNonCoreThread = true;
			}
			condition.notify_one();
		}

		if (createCoreThread)
		{
			std::thread(&JobScheduler::workerFunction, true).detach();
		}
		else if (createNonCoreThread)
		{
			std::thread(&JobScheduler::workerFunction, false).detach();
		}
	}

	void JobScheduler::shutdown()
	{
		{
			std::unique_lock lock(mutex);
			if (isShuttingDown)
			{
				return;
			}
			isShuttingDown = true;
		}
		condition.notify_all();

		std::unique_lock lock(mutex);
		shutdownCondition.wait(lock, []() { return threadCount == 0; });
	}

	void JobScheduler::workerFunction(bool isCore)
	{
		while (true)
		{
			std::function<void()> task;
			{
				std::unique_lock lock(mutex);

				if (isCore)
				{
					condition.wait(lock,
								   []() { return !taskQueue.empty() || isShuttingDown; });
				}
				else
				{
					auto status = condition.wait_for(
						lock, keepAliveTime,
						[]() { return !taskQueue.empty() || isShuttingDown; });
					if (!status)
					{
						if (taskQueue.empty() && !isShuttingDown)
						{
							threadCount--;
							shutdownCondition.notify_one();
							return;
						}
					}
				}

				if (isShuttingDown && taskQueue.empty())
				{
					threadCount--;
					shutdownCondition.notify_one();
					return;
				}

				if (!taskQueue.empty())
				{
					task = taskQueue.front();
					taskQueue.pop();
				}
			}

			if (task)
			{
				try
				{
					task();
				}
				catch (std::exception& e)
				{
					log_fatal("an error ocurred while executing job: %s", e.what());
				}
			}
		}
	}
}