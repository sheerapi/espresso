#pragma once
#include "Thread.h"
#include "utils/Demangle.h"
#include <condition_variable>
#include <memory>
#include <mutex>
#include <vector>

namespace platform
{
	class ThreadManager
	{
	public:
		template <typename T> static auto addThread() -> T*
		{
			if (!typeCheck<Thread, T>())
			{
				return nullptr;
			}

			auto thread = std::unique_ptr<T>(new T());
			threads.push_back(thread);

			log_trace("adding %s thread (now %d threads)",
					  demangle(typeid(T).name()).c_str(), threads.size() - 1);

			return thread.get();
		}

		static auto getThread(int index) -> Thread*;
		static void run();
		static void shutdown();

		static auto getCv() -> std::condition_variable&;
        static auto isReady() -> bool;
		static auto isRunning() -> bool;
		static void reportShutdown();

	private:
		static std::vector<std::unique_ptr<Thread>> threads;
		static std::mutex mutex;
		static std::condition_variable cv;
		static std::atomic<bool> ready;
		static std::atomic<bool> running;
		static std::atomic<int> shutdowns;
	};
}