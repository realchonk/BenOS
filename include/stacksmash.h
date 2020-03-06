#ifndef FILE_STACKSMASH_H
#define FILE_STACKSMASH_H
#include <stdint.h>

extern const uintptr_t __stack_chk_guard;

#ifdef __cplusplus
extern "C"
#endif
__attribute__((noreturn))
void __stack_chk_fail(void);

#endif /* FILE_STACKSMASH_H */