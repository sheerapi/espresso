#include "core/jobs/JobManager.h"
#include "core/log.h"
#include <thread>

namespace core::jobs
{
    void JobManager::initialize()
    {
        // reserve 1 for main
        int numThreads = (int)std::thread::hardware_concurrency() - 1;

        log_info("initialized job manager with %d worker threads", numThreads);
    }

    void JobManager::shutdown()
    {

    }
}