#ifndef FILE_HEAP_HPP
#define FILE_HEAP_HPP
#include <stddef.h>

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
}


#endif /* FILE_HEAP_HPP */