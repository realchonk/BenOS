#ifndef FILE_ARCH_PAGING_H
#define FILE_ARCH_PAGING_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vmm_context vmm_context;

vmm_context* vmm_create_context(void);
void vmm_activate_context(vmm_context* ctx);
void vmm_map_page(vmm_context* ctx, uintptr_t vaddr, uintptr_t paddr, bool usermode);
void vmm_free_page(vmm_context* ctx, uintptr_t vaddr);
void vmm_flush_page(volatile void* vaddr);
void vmm_flush(vmm_context* ctx);
void* vmm_get_phys(const vmm_context* ctx, uintptr_t vaddr);
void vmm_enable(void);

#ifdef __cplusplus
}
#endif

#endif /* FILE_ARCH_PAGING_H */