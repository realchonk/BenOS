#include "arch/i386/x86task.h"
#include "stdlib.h"
#include "string.h"
#include "printk.h"


struct taskmgr_entry {
	task_t* task;
	struct taskmgr_entry* next;
	struct taskmgr_entry* prev;
	pid_t pid;
	bool waiting;
	pid_t wpid;
	int retVal;
	struct taskmgr_entry* parent;
};

static uint8_t pid_bitmap[65536 / 8] = { 0 };
static struct taskmgr_entry* first_task = NULL;
static struct taskmgr_entry* cur_task = NULL;
static pid_t next_pid(void) {
	for (pid_t i = 0; i < sizeof(pid_bitmap); ++i) {
		for (uint8_t j = 0; j < 8; ++j) {
			const uint8_t mask = 1 << j;
			if ((pid_bitmap[i] & mask) == 0) {
				pid_bitmap[i] |= mask;
				return (i * 8) | j; 
			}
		}
	}
	return 0xffff;
}
static void free_pid(pid_t pid) {
	pid_bitmap[pid / 8] &= ~(1 << (pid % 8));
}
static bool is_pid_used(pid_t pid) {
	return pid_bitmap[pid / 8] & (1 << (pid % 8));
}

void taskmgr_init(void) {
	memset(pid_bitmap, 0, sizeof(pid_bitmap));
	first_task = cur_task = NULL;
}
extern void no_task_handler(void);
static struct taskmgr_entry* get_entry(pid_t pid);
struct cpu_state* taskmgr_schedule(struct cpu_state* cpustate) {
	if (!first_task) {
		no_task_handler();
		asm("cli");
		asm("hlt");
	}
	if (!cur_task) cur_task = first_task;
	else cur_task->task->cpustate = cpustate, cur_task = cur_task->next;
	if (!cur_task) cur_task = first_task;
	struct cpu_state* s = cur_task->task->cpustate;
	if (cur_task->waiting && is_pid_used(cur_task->wpid)) {
		struct taskmgr_entry* e = get_entry(cur_task->wpid);
		if (e) s = e->task->cpustate, cur_task = e;
		else cur_task->waiting = false;
	} 
	return s;
}
pid_t taskmgr_mypid(void) {
	return cur_task->pid;
}
int taskmgr_waitfor(pid_t pid) {
	cur_task->waiting = true;
	cur_task->wpid = pid;
	asm volatile("int $0x20");
	return cur_task->retVal;
}
static struct taskmgr_entry* get_entry(pid_t pid) {
	for (struct taskmgr_entry* e = first_task; e; e = e->next) {
		if (e->pid == pid) return e;
	}
	return NULL;
}
task_t* taskmgr_get(pid_t pid) {
	struct taskmgr_entry* e = get_entry(pid);
	return e ? e->task : NULL;
}
void taskmgr_exit(int ec) {
	if (cur_task->next) cur_task->next->prev = cur_task->prev;
	if (cur_task->prev) cur_task->prev->next = cur_task->next;
	else first_task = cur_task->next;
	if (cur_task->parent) cur_task->parent->retVal = ec;
	free_pid(cur_task->pid);
	free(cur_task);
	cur_task = NULL;
	asm volatile("int $0x20");
}

extern uint16_t i386_gdt_codeSegmentSelector();
extern uint16_t i386_gdt_ucodeSegmentSelector();
extern uint16_t i386_gdt_dataSegmentSelector();
extern uint16_t i386_gdt_udataSegmentSelector();
task_t* taskmgr_create_task(void(*entry)()) {
	task_t* task = (task_t*)malloc(sizeof(task_t));
	if (!task) {
		log(ERROR, "Couldn't allocate task_t");
		return NULL;
	}
	memset((void*)task->stack, 0, 8192 - sizeof(struct cpu_state));
	task->cpustate = (struct cpu_state*)(&task->stack[sizeof(task->stack) - sizeof(struct cpu_state)]);
	memset(task->cpustate, 0, sizeof(struct cpu_state));
	task->cpustate->eip = (uint32_t)entry;
	task->cpustate->eflags = 0x202;
	task->cpustate->cs = i386_gdt_codeSegmentSelector();
	task->cpustate->ds = i386_gdt_dataSegmentSelector();
	/*task->cpustate->es = i386_gdt_dataSegmentSelector();
	task->cpustate->fs = i386_gdt_dataSegmentSelector();
	task->cpustate->gs = i386_gdt_dataSegmentSelector();
	*/return task;
}
void taskmgr_free_task(task_t* task) {
	free(task->cpustate);
	free(task);
}
pid_t taskmgr_add(task_t* task) {
	if (!task) return -1;
	struct taskmgr_entry* e = (struct taskmgr_entry*)malloc(sizeof(struct taskmgr_entry));
	if (first_task) {
		e->next = first_task;
		first_task->prev = e;
		first_task = e;
	}
	else {
		first_task = e;
		e->next = NULL;
	}
	e->wpid = 0;
	e->waiting = false;
	e->parent = cur_task;
	e->prev = NULL;
	e->task = task;
	const pid_t pid = e->pid = next_pid();
	return pid;
}
task_t* taskmgr_remove(pid_t pid) {
	if (!first_task) return NULL;
	for (struct taskmgr_entry* e = first_task; e; e = e->next) {
		if (e->pid == pid) {
			task_t* task = e->task;
			if (e->next) e->next->prev = e->prev;
			if (e->prev) e->prev->next = e->next;
			else first_task = e->next;
			free_pid(e->pid);
			free(e);
			return task;
		}
	}
	return NULL;	
}
size_t taskmgr_count(void) {
	size_t n = 0;
	for (struct taskmgr_entry* e = first_task; e; ++n, e = e->next);
	return n;
}