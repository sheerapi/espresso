#pragma once
#include "core/jobs/JobTypes.h"
#include <cstddef>
#include <cstdint>
#include <deque>
#include <memory>
#include <atomic>

namespace core::jobs
{
	class FrameMemoryPool
	{
	public:
		FrameMemoryPool(size_t initialSize = FrameMemorySize); // 4MB default
		~FrameMemoryPool() = default;

		auto allocate(size_t size, size_t alignment = alignof(std::max_align_t)) -> void*;
		void reset();

		[[nodiscard]] auto getBytesUsed() const -> size_t;
		[[nodiscard]] auto getBytesRemaining() const -> size_t;

		struct Stats
		{
			size_t currentSize = 0;
			size_t usedThisFrame = 0;
			size_t allocationsThisFrame = 0;

			size_t peakBytesUsed = 0;
			size_t totalAllocations = 0;
			size_t totalBytesAllocated = 0;
			size_t resizesPerformed = 0;
		};

		[[nodiscard]] auto getStats() const -> const Stats&;

	private:
		void _resize(size_t newSize);

		std::unique_ptr<uint8_t[]> _memory;
		std::atomic<size_t> _offset;
		size_t _size;

		int _overuseCount = 0;
		int _underuseCount = 0;
		int _framesSinceLastResize = 0;

		const int ResizeCooldownFrames = 32;
		const int ShrinkCooldownFrames = 64;
		const int GrowThreshold = 3;
		const float GrowUsageThreshold = 0.85F;
		const float ShrinkUsageThreshold = 0.4F;

		// Sliding window for peak tracking
		std::deque<size_t> _recentUsage;
		const int UsageWindow = 60;

		Stats _stats;
	};
}