#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

namespace core::jobs
{
	class JobMemoryPool
	{
	public:
		explicit JobMemoryPool(size_t poolSize);
		~JobMemoryPool();

		auto allocate(size_t size, size_t alignment = alignof(std::max_align_t)) -> void*;
		void deallocate(void* ptr);
		void reset();

	private:
		struct Block
		{
			void* ptr;
			size_t size;
			bool free;
		};

		std::unique_ptr<uint8_t[]> _memory;
		std::vector<Block> _blocks;
		size_t _size;
		std::mutex _mutex;
	};
}