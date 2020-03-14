#ifndef FILE_TASK_H
#define FILE_TASK_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct cpu_state;
typedef struct task {
	volatile uint8_t stack[8192];
	struct cpu_state* cpustate;
} task_t;

typedef uint16_t pid_t;

void taskmgr_init(void);
task_t* taskmgr_create_task(void(*entry)());
void taskmgr_free_task(task_t* task);
pid_t taskmgr_add(task_t* task);
task_t* taskmgr_remove(pid_t pid);
struct cpu_state* taskmgr_schedule(struct cpu_state* cpustate);
size_t taskmgr_count(void);
int taskmgr_waitfor(pid_t pid);
void taskmgr_exit(int ec);
task_t* taskmgr_get(pid_t pid);
pid_t taskmgr_mypid(void);

#ifdef __cplusplus
}
#endif

#endif /* FILE_TASK_H */