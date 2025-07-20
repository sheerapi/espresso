#include "core/jobs/FrameMemoryPool.h"
#include "core/log.h"
#include <algorithm>
#include <cstring>

namespace core::jobs
{
	FrameMemoryPool::FrameMemoryPool(size_t initialSize)
		: _offset(0), _size((initialSize + 4095) & ~4095)
	{
		_memory = std::make_unique<uint8_t[]>(_size);
		_stats.currentSize = _size;
	}

	auto FrameMemoryPool::allocate(size_t size, size_t alignment) -> void*
	{
		size_t currentOffset = _offset.load(std::memory_order_relaxed);
		size_t alignedOffset = (currentOffset + alignment - 1) & ~(alignment - 1);
		size_t newOffset = alignedOffset + size;

		if (newOffset > _size)
		{
			return nullptr;
		}

		if (!_offset.compare_exchange_weak(currentOffset, newOffset,
										   std::memory_order_relaxed))
		{
			return nullptr;
		}

		// update stats
		size_t used = newOffset;
		_stats.usedThisFrame = std::max(_stats.usedThisFrame, used);
		_stats.peakBytesUsed = std::max(_stats.peakBytesUsed, used);
		_stats.allocationsThisFrame++;
		_stats.totalAllocations++;
		_stats.totalBytesAllocated += size;

		return _memory.get() + alignedOffset;
	}

	void FrameMemoryPool::reset()
	{
		_framesSinceLastResize++;

		// track usage
		size_t used = _offset.exchange(0, std::memory_order_relaxed);
		_recentUsage.push_back(used);
		if (_recentUsage.size() > UsageWindow)
		{
			_recentUsage.pop_front();
		}

		float usage = static_cast<float>(used) / static_cast<float>(_size);

		// growing
		if (usage > GrowUsageThreshold)
		{
			_overuseCount++;
			_underuseCount = 0;
		}
		else if (usage < ShrinkUsageThreshold)
		{
			_underuseCount++;
			_overuseCount = 0;
		}
		else
		{
			_overuseCount = 0;
			_underuseCount = 0;
		}

		if (_overuseCount >= GrowThreshold &&
			_framesSinceLastResize >= ResizeCooldownFrames)
		{
            log_trace("frame memory pool wasn't enough, expanding");
			_resize(_size * 2);
			_overuseCount = 0;
			_framesSinceLastResize = 0;
			_recentUsage.clear();
		}

		// shrinking
		if (_framesSinceLastResize >= ShrinkCooldownFrames)
		{
			size_t peakRecent =
				*std::max_element(_recentUsage.begin(), _recentUsage.end());
			if (peakRecent < static_cast<size_t>(_size * ShrinkUsageThreshold) &&
				_size > MinFrameMemorySize)
			{
                log_info("overstepped with frame memory pool, shrinking");
				_resize(_size / 2);
				_underuseCount = 0;
				_framesSinceLastResize = 0;
				_recentUsage.clear();
			}
		}
	}

	void FrameMemoryPool::_resize(size_t newSize)
	{
		if (newSize < MinFrameMemorySize)
		{
			return;
		}

		newSize = (newSize + 4095) & ~4095;
		auto newMemory = std::make_unique<uint8_t[]>(newSize);
		size_t used = std::min(_offset.load(std::memory_order_relaxed), newSize);
		std::memcpy(newMemory.get(), _memory.get(), used);

		_memory = std::move(newMemory);
		_size = newSize;
		_offset.store(used, std::memory_order_relaxed);

		_stats.currentSize = _size;
		_stats.resizesPerformed++;
	}

	auto FrameMemoryPool::getBytesUsed() const -> size_t
	{
		return _offset.load(std::memory_order_relaxed);
	}

	auto FrameMemoryPool::getBytesRemaining() const -> size_t
	{
		return _size - _offset.load(std::memory_order_relaxed);
	}

	auto FrameMemoryPool::getStats() const -> const FrameMemoryPool::Stats&
	{
		return _stats;
	}
}