#include "stdlib.h"
#include "heap.hpp"
#include "printk.h"

namespace mm {

	BlockAllocator::BlockAllocator(void* ptr, size_t num_blocks, size_t size_block) noexcept
		: data((uint8_t*)ptr), num_blocks(num_blocks), size_block(size_block) {
		allocated = (uint8_t*)calloc(1, (num_blocks + 7) / 8);
	}
	BlockAllocator::BlockAllocator(BlockAllocator&& a) noexcept
		: data(a.data), num_blocks(a.num_blocks), size_block(a.size_block) {
		a.data = a.allocated = nullptr;
	}
	BlockAllocator::~BlockAllocator() noexcept {
		if (allocated) ::free(allocated);
	}


	void BlockAllocator::set_block(size_t block, bool val) noexcept {
		uint8_t b = allocated[block / 8];
		const uint8_t i = 1 << (block % 8);
		if (val) b |= i;
		else b &= ~i;
		allocated[block / 8] = b;
	}
	bool BlockAllocator::is_allocated(size_t block) const noexcept {
		const uint8_t b = allocated[block / 8];
		const uint8_t i = 1 << (block % 8);
		return (b & i) == i;
	}
	void* BlockAllocator::allocate() noexcept {
		for (size_t i = 0; i < num_blocks; ++i) {
			if (!is_allocated(i)) {
				set_block(i, true);
				return data + (i * size_block);
			}
		}
		return nullptr;
	}
	void BlockAllocator::free(void* ptr) noexcept {
		const size_t b = ((size_t)ptr - (size_t)data) / size_block;
		if (b < num_blocks) set_block(b, false);
	}

	size_t BlockAllocator::total() const noexcept {
		return num_blocks * size_block;
	}
	size_t BlockAllocator::used() const noexcept {
		size_t num = 0;
		for (size_t i = 0; i < num_blocks; ++i) {
			if (is_allocated(i)) num += size_block;
		}
		return num;
	}
}