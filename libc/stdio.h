#ifndef FILE_LIBC_STDIO_H
#define FILE_LIBC_STDIO_H

#ifdef __cplusplus
extern "C" {
#endif

int putchar(int ch);
int puts(const char* str);
int printf(const char* str, ...);

#ifdef __cplusplus
}
#endif

#endif /* FILE_LIBC_STDIO_H */