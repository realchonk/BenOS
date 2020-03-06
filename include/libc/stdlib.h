#ifndef FILE_LIBC_STDLIB_H
#define FILE_LIBC_STDLIB_H
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct div_t {
	int quot;
	int rem;
} div_t;
typedef struct ldiv_t {
	long quot;
	long rem;
} ldiv_t;
typedef struct lldiv_t {
	long long quot;
	long long rem;
} lldiv_t;

int rand(void);
void srand(unsigned seed);

void* malloc(size_t num);
void* calloc(size_t num, size_t size);
void* realloc(void* ptr, size_t num);
void free(void* ptr);

div_t div(int number, int denom);
ldiv_t ldiv(long number, long denom);
lldiv_t lldiv(long long number, long long denom);

int abs(int x);
long labs(long x);

#ifdef __cplusplus
}
#endif

#endif /* FILE_LIBC_STDLIB_H */