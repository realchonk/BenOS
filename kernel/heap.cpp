#include "string.h"
#include "heap.hpp"
#include "printk.h"

namespace mm {
	HeapManager::HeapManager(void* ptr, size_t size)
		: first((HeapChunk*)ptr), size(size) {
		if (size <= sizeof(HeapChunk)) {
			log(ERROR, "Not enough Memory for HeapManager");
			return;
		}
		first->next = first->prev = nullptr;
		first->size = size - sizeof(HeapChunk);
		first->allocated = false;
	}
	size_t HeapManager::usedHeap() const {
		size_t num = 0;
		for (HeapChunk* chunk = first; chunk; chunk = chunk->next) {
			if (chunk->allocated)
				num += chunk->size + sizeof(HeapChunk);
		}
		return num;
	}

	void* HeapManager::allocate(size_t size) {
		HeapChunk* result = nullptr;
		for (HeapChunk* chunk = first; chunk; chunk = chunk->next) {
			if (chunk->size >= size && !chunk->allocated) {
				if (!result || chunk->size < result->size)
					result = chunk;
			}
		}
		if (!result) return nullptr;
		if (result->size >= size + sizeof(HeapChunk) + sizeof(size_t)) {
			HeapChunk* tmp = (HeapChunk*)((size_t)result + sizeof(HeapChunk) + size);
			tmp->allocated = false;
			tmp->size = result->size - size - sizeof(HeapChunk);
			tmp->prev = result;
			tmp->next = result->next;
			if (tmp->next)
				tmp->next->prev = tmp;
			result->next = tmp;
			result->size = size;
		}
		result->allocated = true;
		return (void*)((size_t)result + sizeof(HeapChunk));
	}
	void HeapManager::free(void* ptr) {
		HeapChunk* chunk = (HeapChunk*)((size_t)ptr - sizeof(HeapChunk));
		chunk->allocated = false;
		if (chunk->prev && !chunk->prev->allocated) {
			chunk->prev->next = chunk->next;
			chunk->prev += chunk->size + sizeof(HeapChunk);
			if (chunk->next)
				chunk->next->prev = chunk->prev;
			chunk = chunk->prev;
		}

		if (chunk->next && !chunk->next->allocated) {
			chunk->size += chunk->next->size + sizeof(HeapChunk);
			chunk->next = chunk->next->next;
			if (chunk->next)
				chunk->next->prev = chunk;
		}
	}
	void* HeapManager::reallocate(void* ptr, size_t size) {
		HeapChunk* chunk = (HeapChunk*)((size_t)ptr - sizeof(HeapChunk));
		if (size > chunk->size) {
			const size_t diff = size - chunk->size;
			if (chunk->next && !chunk->next->allocated && diff < chunk->next->size + sizeof(HeapChunk)) {
				if (chunk->next->size - diff >= sizeof(HeapChunk) + sizeof(size_t)) {
					HeapChunk* tmp = (HeapChunk*)((size_t)chunk->next + diff);
					tmp->prev = chunk;
					tmp->next = chunk->next->next;
					if (tmp->next) tmp->next->prev = tmp;
					chunk->size = size;
					tmp->size -= diff;
					tmp->allocated = false;	
					chunk->next = tmp;
					return ptr;
				}
				else {
					chunk->size += chunk->next->size + sizeof(HeapChunk);
					chunk->next = chunk->next->next;
					if (chunk->next) chunk->next->prev = chunk;
					return ptr;
				}
			}
			else {
				void* new_ptr = allocate(size);
				if (!new_ptr) return nullptr;
				memcpy(new_ptr, ptr, size);
				free(ptr);
				return new_ptr;
			}
		}
		else if (chunk->size - size >= sizeof(HeapChunk) + sizeof(size_t)) {
			HeapChunk* tmp = (HeapChunk*)((size_t)chunk + chunk->size);
			tmp->next = chunk->next;
			tmp->prev = chunk;
			if (tmp->next) tmp->next->prev = tmp;
			chunk->next = tmp;
			tmp->size = chunk->size - size - sizeof(HeapChunk);
			chunk->size = size;
			return ptr;
		}
		else return ptr;
	}

	HeapManager* heapmgr;
}
extern "C" {
	void* malloc(size_t num) {
		return mm::heapmgr ? mm::heapmgr->allocate(num) : NULL;
	}
	void* calloc(size_t num, size_t size) {
		void* ptr = malloc(num * size);
		return ptr ? memset(ptr, 0, num * size) : NULL;
	}
	void* realloc(void* ptr, size_t num) {
		return mm::heapmgr ? mm::heapmgr->reallocate(ptr, num) : NULL;
	}
	void free(void* ptr) {
		if (mm::heapmgr) mm::heapmgr->free(ptr);
	}
}

void* operator new(size_t size) { return malloc(size); }
void* operator new[](size_t size) { return malloc(size); }
void operator delete(void* ptr) { free(ptr); }
void operator delete[](void* ptr) { free(ptr); }