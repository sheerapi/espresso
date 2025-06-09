#include "platform/Thread.h"
#include "SDL_timer.h"
#include "core/log.h"
#include "utils/Demangle.h"
#include <typeinfo>

namespace platform
{
	std::atomic<int> Thread::threadCount;

	Thread::Thread()
	{
		thread = std::thread([=]() { _run(); });

		threadCount++;

		if (threadCount >= std::thread::hardware_concurrency() * 2)
		{
			log_warn("thread limit hit, consider de-spawning some threads");
		}
	}

	void Thread::_run()
	{
		try
		{
			init();
            executeWorkQueue();

			while (running)
			{
				time.startMeasure();

                executeWorkQueue();
				update();

                time.endMeasure();
			}

			shutdown();
		}
		catch (std::exception& e)
		{
			log_error("An error ocurred in %s: %s",
					  demangle(typeid(*this).name()).c_str(), e.what());
		}
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

    void Thread::executeWorkQueue()
    {
		std::lock_guard<std::mutex> lock(mutex);
		while (!workQueue.empty())
		{
			workQueue.front()();
			workQueue.pop();
		}
	}

    ThreadTime::ThreadTime()
	{
		_last = 0;
		_now = SDL_GetPerformanceCounter();
	}

    void ThreadTime::startMeasure()
	{
		_last = _now;
	}

    void ThreadTime::endMeasure()
	{
		_now = SDL_GetPerformanceCounter();
		_delta = ((static_cast<double>(_now) - static_cast<double>(_last)) * 1000 /
				  (double)SDL_GetPerformanceFrequency()) *
				 0.001F;
		_elapsed += _delta;
		_frames++;
	}

	auto ThreadTime::getTPS() const -> double
	{
		double safeTPS = 0;

		if (getDelta() > 0)
		{
			if (getDelta() < 1e-6F)
			{
				safeTPS = 1.0F / 1e-6F;
			}
			else
			{
				safeTPS = 1.0F / getDelta();
			}
		}
		else
		{
			safeTPS = 0;
		}

		return safeTPS;
	}

	auto ThreadTime::getDelta() const -> double
    {
        return _delta;
    }

	auto ThreadTime::getElapsed() const -> double
    {
        return _elapsed;
    }

	auto ThreadTime::getFrames() const -> uint64_t
    {
        return _frames;
    }

    auto Thread::getTime() -> ThreadTime&
    {
        return time;
    }

	void Thread::submitTask(const std::function<void()>& task)
	{
		std::lock_guard<std::mutex> lock(mutex);
		workQueue.push(task);
	}
}