#include "platform/Thread.h"
#include "core/log.h"

namespace platform
{
	std::atomic<int> Thread::threadCount;

    Thread::Thread()
	{
        thread = std::thread([=](){
            _run();
        });

        threadCount++;

        if (threadCount >= std::thread::hardware_concurrency() * 2)
        {
            log_warn("thread limit hit, consider de-spawning some threads");
        }
    }

    void Thread::_run()
    {
        init();

        while (running)
        {
            update();
        }

        shutdown();
    }

    Thread::~Thread()
    {
        terminate();
    }

    void Thread::terminate()
    {
		running = false;
		join();
	}

    void Thread::detach()
    {
        thread.detach();
    }

    void Thread::join()
    {
		if (thread.joinable())
		{
			thread.join();
		}
	}
}