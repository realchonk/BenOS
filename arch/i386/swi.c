#include <stdint.h>
#include "syscall.h"
#include "task.h"

void taskmgr_remove_this(void);
uint32_t handle_swi_0xA0(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi) {
	switch (eax) {
	case 0: // sys_write
		return sys_write((int)ecx, (const void*)edx, (size_t)esi);
	case 1: // sys_read
		return sys_read((int)ecx, (void*)edx, (size_t)esi);
	case 2: // sys_exit
		sys_exit((int)ecx);
		return 0;
	case 3: // sys_exec
		return sys_exec((void(*)())ecx);
	case 4: // sys_waitfor
		sys_waitfor((pid_t)ecx);
		return 0;
	case 5: // exception
		sys_excep((const char*)ecx);
		return 0;
	case 6: // sys_getpid
		return sys_getpid();
	default: return -1;
	}
}