#include "stacksmash.h"
#include "printk.h"


#if UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD 0xe2dee396
#else
#define STACK_CHK_GUARD 0x595e9fbd94fda766
#endif

const uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

__attribute__((noreturn))
void abort(void);

void __stack_chk_fail(void) {
	printk("stack smashing detected.\n");
	abort();
}
