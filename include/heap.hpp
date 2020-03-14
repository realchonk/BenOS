#ifndef FILE_HEAP_HPP
#define FILE_HEAP_HPP
#include <stddef.h>
#include <stdint.h>

namespace mm {
	struct HeapChunk {
		HeapChunk* next;
		HeapChunk* prev;
		size_t size;
		bool allocated;
	};
	class HeapManager {
	private:
		HeapChunk* first;
		size_t size;
	public:
		HeapManager(void* ptr, size_t size);
		HeapManager(const HeapManager&) = delete;

		HeapManager& operator=(const HeapManager&) = delete;
		HeapManager& operator=(HeapManager&&) = delete;

		void* allocate(size_t size);
		void* reallocate(void* ptr, size_t size);
		void free(void* ptr);

		size_t totalHeap() const { return size; }
		size_t usedHeap() const;
	};
	class BlockAllocator {
	private:
		uint8_t* allocated;
		uint8_t* data;
	public:
		const size_t num_blocks;
		const size_t size_block;
		BlockAllocator(void* ptr, size_t num_blocks, size_t size_block) noexcept;
		BlockAllocator(const BlockAllocator&) = delete;
		BlockAllocator(BlockAllocator&& a) noexcept;
		BlockAllocator& operator=(const BlockAllocator&) = delete;
		BlockAllocator& operator=(BlockAllocator&&) = delete;
		~BlockAllocator() noexcept;

		operator bool() const noexcept { return data && allocated && num_blocks && size_block; }

		void* allocate() noexcept;
		void free(void* ptr) noexcept;

		size_t total() const noexcept;
		size_t used() const noexcept;
	private:
		void set_block(size_t block, bool val) noexcept;
		bool is_allocated(size_t block) const noexcept;
	};
}


#endif /* FILE_HEAP_HPP */