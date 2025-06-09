#pragma once
#include <atomic>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

namespace platform
{
	class ThreadTime
	{
	public:
		ThreadTime();

		void startMeasure();
		void endMeasure();

		[[nodiscard]] auto getDelta() const -> double;
		[[nodiscard]] auto getElapsed() const -> double;
		[[nodiscard]] auto getFrames() const -> unsigned long;

		[[nodiscard]] auto getTPS() const -> double;

	private:
		unsigned long _last{0};
		unsigned long _now{0};
		uint64_t _frames{0};
		double _delta{0};
		double _elapsed{0};
	};

	class Thread
	{
	public:
		static std::atomic<int> threadCount;

		Thread();
		virtual ~Thread();

		virtual void init() {};
		virtual void update() {};
		virtual void shutdown() {};

		void detach();
		void join();
		void terminate();

		void submitTask(const std::function<void()>& task);
        auto getTime() -> ThreadTime&;

	protected:
		std::thread thread;
		std::mutex mutex;
		std::atomic<bool> running{true};
		ThreadTime time;
		std::queue<std::function<void()>> workQueue;

		void executeWorkQueue();

	private:
		void _run();
	};
}