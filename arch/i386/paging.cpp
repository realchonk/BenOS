#include <stddef.h>
#include "paging.h"
#include "stdlib.h"
#include "string.h"
#include "printk.h"
#include "heap.hpp"

#define PDE_PRESENT	0x1
#define PDE_RW		0x2
#define PDE_USER	0x4

#define PTE_PRESET	0x1
#define PTE_RW		0x2
#define PTE_USER	0x4

static volatile uint8_t pagebuf[64 * 4096] __attribute__((aligned(4096))) {};
static mm::BlockAllocator* page_alloc = NULL;

extern "C" {

struct vmm_context {
	volatile uint32_t* pagedir;
};
vmm_context* vmm_create_context(void) {
	if (!page_alloc) {
		page_alloc = new mm::BlockAllocator((void*)pagebuf, 16384, 4096);
	}
	vmm_context* ctx = new vmm_context;
	if (!ctx) return NULL;
	ctx->pagedir = (volatile uint32_t*)page_alloc->allocate();
	if (!ctx->pagedir) {
		log(ERROR, "Couldn't allocate page directory");
		free(ctx);
		return NULL;
	}
	memset((void*)ctx->pagedir, 0, 4096);
	return ctx;
}
void vmm_activate_context(vmm_context* ctx) {
	asm volatile("mov %0, %%cr3" : : "r"(ctx->pagedir));
	log(DEBUG, "switched context");
}
void vmm_flush(vmm_context* ctx) {
	vmm_activate_context(ctx);
}
void vmm_map_page(vmm_context* ctx, uintptr_t vaddr, uintptr_t paddr, bool usermode) {
	const uint16_t pdi = (vaddr >> 22) & 0x3ff;
	const uint16_t pti = (vaddr >> 12) & 0x3ff;

	volatile uint32_t* const pdir = ctx->pagedir;
	if (!(pdir[pdi] & 0x1)) {
		pdir[pdi] = ((uint32_t)page_alloc->allocate() & ~0x3ff) | PDE_PRESENT | PDE_RW;
	}
	volatile uint32_t* const ptbl = (volatile uint32_t*)(pdir[pdi] & ~0x3ff); 
	ptbl[pti] = (paddr & ~0x3ff) | PTE_PRESET | PTE_RW;
	if (usermode) ptbl[pti] |= PTE_USER;
}
void vmm_free_page(vmm_context* ctx, uintptr_t vaddr) {
	const uint16_t pdi = (vaddr >> 22) & 0x3ff;
	const uint16_t pti = (vaddr >> 12) & 0x3ff;
	volatile uint32_t* const pdir = ctx->pagedir;
	if (pdir[pdi] & 0x1) {
		volatile uint32_t* const ptbl = (volatile uint32_t*)(pdir[pdi] & ~0x3ff);
		ptbl[pti] = 0;

		bool empty = true;
		for (uint16_t i = 0; i < 1024; ++i) {
			if (ptbl[pti] & 0x1) {
				empty = false;
				break;
			}
		}
		if (empty) page_alloc->free((void*)(pdir[pdi] & ~0x3ff));
	}
}
void* vmm_get_phys(const vmm_context* ctx, uintptr_t vaddr) {
	const uint16_t pdi = (vaddr >> 22) & 0x3ff;
	const uint16_t pti = (vaddr >> 12) & 0x3ff;
	const volatile uint32_t* const pdir = ctx->pagedir;
	if (pdir[pdi] & 0x1) {
		const volatile uint32_t* const ptbl = (volatile uint32_t*)(pdir[pdi] & ~0x3ff);
		return (ptbl[pti] & 0x1) ? (void*)(ptbl[pti] & ~0x3ff) : NULL;
	}
	else return NULL;
}
}