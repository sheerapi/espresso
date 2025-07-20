#pragma once
#include <cstddef>
#include <cstdint>

namespace core::jobs
{
	using JobID = uint64_t;
	using FenceID = uint64_t;
	using FrameID = uint64_t;
	using ThreadID = uint32_t;

	constexpr JobID InvalidJobID = 0;
	constexpr FenceID InvalidFenceID = 0;
	constexpr size_t MaxWorkerThreads = 64;
	constexpr size_t MaxJobsPerFrame = 2048;

	enum class JobPriority : uint8_t
	{
		Critical = 0,  // frame-critical
		High = 1,	   // well, important
		Normal = 2,	   // regular
		Low = 3,	   // background tasks
		Background = 4 // asset loading, profiling, whatever
	};

	enum class JobState : uint8_t
	{
		Created,
		Running,
		Waiting,
		Completed,
		Failed
	};
}