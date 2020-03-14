#include "arch/i386/serial.h"
#include "terminal.hpp"
#include "syscall.h"
#include "string.h"
#include "task.h"

extern "C" {
	void taskmgr_remove_this();
	int sys_write(int fd, const void* ptr, size_t size) {
		const char* data = (const char*)ptr;
		size_t i = 0;
		switch (fd) {
		case 0: // terminal
			for (; i < size; ++i) terminal::putc(data[i]);
			return i;
		case 2: // serial
			for (; i < size; ++i) serial_write(data[i]);
			return i;
		default: return 0;
		}
	}
	int sys_read(int fd, void* ptr, size_t size) {
		char* data = (char*)ptr;
		size_t i = 0;
		switch (fd) {
		case 2: // serial
			for (; i < size; ++i) data[i] = serial_read();
			return i;
		default: return 0;
		}
	}
	void sys_exit(int ec) {
		taskmgr_exit(ec);
	}
	pid_t sys_exec(void(*func)()) {
		task_t* task = taskmgr_create_task(func);
		if (!task) return 0xffff;
		return taskmgr_add(task);
	}
	int sys_waitfor(pid_t pid) {
		return taskmgr_waitfor(pid);
	}
	void sys_excep(const char* ecx) {
		sys_write(2, ecx, strlen(ecx));
		sys_exit(255);
	}
	pid_t sys_getpid(void) {
		return taskmgr_mypid();
	}
}