#include "platform/ThreadManager.h"
#include "core/EventManager.h"
#include <cassert>

namespace platform
{
	std::vector<std::unique_ptr<Thread>> ThreadManager::threads;
	std::mutex ThreadManager::mutex;
	std::condition_variable ThreadManager::cv;
	std::atomic<bool> ThreadManager::ready;
	std::atomic<bool> ThreadManager::running;
	std::atomic<int> ThreadManager::shutdowns;

	void ThreadManager::run()
	{
		std::lock_guard<std::mutex> lock(mutex);
		ready = true;
		running = true;
		cv.notify_all();
	}

    void ThreadManager::shutdown()
    {
		std::unique_lock<std::mutex> lock(mutex);
		running = false;

		for (auto& thread : threads)
		{
            thread->terminate();
			core::EventManager::triggerEvent(
				"thread.shutdown",
				(void*)demangle(typeid(*thread).name()).c_str()); // NOLINT
		}

		log_debug("waiting for threads to terminate...");
		cv.wait(lock, []() { return shutdowns.load() == (int)threads.size(); });

		threads.clear();
	}

	auto ThreadManager::getThread(int index) -> Thread*
    {
		assert((int)threads.size() > index);
        return threads[index].get();
	}

	auto ThreadManager::getCv() -> std::condition_variable&
    {
        return cv;
    }

	auto ThreadManager::isReady() -> bool
    {
        return ready;
    }

	auto ThreadManager::isRunning() -> bool
    {
        return running;
    }

	void ThreadManager::reportShutdown()
    {
        shutdowns++;
    }
}