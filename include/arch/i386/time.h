#ifndef FILE_ARCH_TIME_H
#define FILE_ARCH_TIME_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct i386_time {
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t day;
	uint8_t month;
	uint16_t year;
} i386_time_t;

i386_time_t i386_get_time(void);
void i386_format_time(char* buf, const i386_time_t* time);
int i386_get_day_of_week(const i386_time_t* time);

#ifdef __cplusplus
}
#endif

#endif /* FILE_ARCH_TIME_H */