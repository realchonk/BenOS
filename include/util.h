#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#define arraylen(a) (sizeof(a) / sizeof(*a))

void out_of_bounds(void);

#ifdef __cplusplus
}
#endif

#endif /* FILE_UTIL_H */