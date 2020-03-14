#ifndef FILE_SYSCALL_H
#define FILE_SYSCALL_H
#include <stddef.h>
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

int sys_write(int fd, const void* ptr, size_t size);
int sys_read(int fd, void* ptr, size_t size);
void sys_exit(int ec);
pid_t sys_exec(void(*func)());
int sys_waitfor(pid_t pid);
void sys_excep(const char* msg);
pid_t sys_getpid(void);

#ifdef __cplusplus
}
#endif

#endif /* FILE_SYSCALL_H */