#ifndef FILE_LIBC_STRING_H
#define FILE_LIBC_STRING_H
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void* memcpy(void* dest, const void* src, size_t num);
void* memset(void* dest, int val, size_t num);
void* memchr(const void* ptr, int val, size_t num);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t num);
char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, size_t num);
char* strchr(const char* str, int ch);

char* strlwr(char* str);
char* strupr(char* str);
char* strrvs(char* str);
char* strnlwr(char* str, size_t num);
char* strnupr(char* str, size_t num);
char* strnrvs(char* str, size_t num);

int memcmp(const void* p1, const void* p2, size_t num);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t num);

size_t strlen(const char* str);
size_t strnlen(const char* str, size_t num);


#ifdef __cplusplus
}
#endif


#endif /* FILE_LIBC_STRING_H */