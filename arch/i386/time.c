#include "arch/i386/port.h"
#include "arch/i386/time.h"
#include "string.h"

#define CENTURY 2000

static i386_time_t time = { 0 };
static void read_time(void) {
	while (cmos_read(true, 0x0a));
	time.second = cmos_read(true, 0x00);
	time.minute = cmos_read(true, 0x02);
	time.hour	= cmos_read(true, 0x04);
	time.day	= cmos_read(true, 0x07);
	time.month	= cmos_read(true, 0x08);
	time.year	= cmos_read(true, 0x09);
}
inline static bool time_is_neq(const i386_time_t* last) {
	if (last->second != time.second) return true;
	else if (last->minute != time.minute) return true;
	else if (last->hour	  != time.hour	) return true;
	else if (last->day	  != time.day	) return true;
	else if (last->month  != time.month	) return true;
	else if (last->year	  != time.year	) return true;
	else return false;
}
i386_time_t i386_get_time(void) {	
	i386_time_t last;
	read_time();
	do {
		last.second = time.second;
		last.minute = time.minute;
		last.hour	= time.hour;
		last.day	= time.day;
		last.month	= time.month;
		last.year	= time.year;
		read_time();
	} while (time_is_neq(&last));
	const uint8_t reg_b = cmos_read(true, 0x08);
	if (!(reg_b & 4)) {
		time.second = (time.second & 0x0f) + ((time.second / 16) * 10);
		time.minute = (time.minute & 0x0f) + ((time.minute / 16) * 10);
		time.hour	= (time.hour   & 0x0f) + ((time.hour   / 16) * 10);
		time.day	= (time.day	   & 0x0f) + ((time.day	   / 16) * 10);
		time.month	= (time.month  & 0x0f) + ((time.month  / 16) * 10);
		time.year	= (time.year   & 0x0f) + ((time.year   / 16) * 10);
	}
	if (!(reg_b & 2) && (time.hour & 0x80))
		time.hour = ((time.hour & 0x7f) + 12) % 24;
	time.year += CENTURY;
	return time;
}

static bool is_leap_year(int year) {
	if ((year % 400) == 0) return true;
	else if ((year % 100) == 0) return false;
	else if ((year % 4) == 0) return true;
	else return false;
}
int i386_get_day_of_week(const i386_time_t* time) {
	static const uint8_t month_codes[12] = { 0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5 };
	static const uint8_t century_codes[7] = { 4, 2, 0, 6, 4, 2, 0 };
	const int century_code = century_codes[(time->year) / 100 - 17];
	const int year_code = ((time->year % 100) + ((time->year % 100) / 4)) % 7;
	const int month_code = month_codes[time->month];
	const bool leap_year = is_leap_year;

	const int tmp = (year_code + month_code + century_code + time->day - leap_year) % 7;
	return leap_year && time->month <= 1 ? tmp - 1 : tmp;
}

static char* u2s(char* str, unsigned val, size_t* i) {
	if (!val) {
		str[(*i)++] = '0';
		str[(*i)++] = '\0';
		return str;
	}
	while (val) {
		str[(*i)++] = (val % 10) + '0';
		val /= 10;
	}
	str[(*i)++] = '\0';
	return strnrvs(str, *i);
}
static const char* daynames[7] = {
	"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun",
};
void i386_format_time(char* buf, const i386_time_t* time) {
	size_t i = 0;
	strcpy(buf, daynames[i386_get_day_of_week(time)]);
	i += 3;
}