#pragma once
#include <atomic>
#include <mutex>
#include <thread>

namespace platform
{
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

    protected:
        std::thread thread;
        std::mutex mutex;
		std::atomic<bool> running{true};

    private:
		void _run();
	};
}