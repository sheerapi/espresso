#pragma once

namespace core::jobs
{
    class JobManager
    {
    public:
        static void initialize();
        static void shutdown();
    };
}