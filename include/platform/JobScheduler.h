#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>

#define es_keepAliveTime 5

namespace internals
{
	class JobScheduler
	{
	public:
		static void init();
		static void submit(const std::function<void()>& task);
		static void shutdown();

	private:
		static void workerFunction(bool isCore);

		inline static std::queue<std::function<void()>> taskQueue;
		inline static std::mutex mutex;
		inline static std::condition_variable condition;
		inline static std::condition_variable shutdownCondition;
		inline static bool isShuttingDown{false};
		inline static int threadCount{0};
		inline static int corePoolSize = 0;
		inline static int maxPoolSize = 0;
		inline static std::chrono::seconds keepAliveTime{es_keepAliveTime};
		inline static int queueThreshold = 0;
	};
}