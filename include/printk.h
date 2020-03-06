#ifndef FILE_PRINTK_H
#define FILE_PRINTK_H
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

enum LogLevel {
	DEBUG,
	INFO,
	WARNING,
	ERROR,
	CRITICAL,
};

int vprintk(const char* format, va_list ap);
int printk(const char* format, ...);
void panic(const char* msg, ...);

void log(enum LogLevel ll, const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif /* FILE_PRINTK_H */