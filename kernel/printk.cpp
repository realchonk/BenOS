#include "arch/i386/serial.h"
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include "terminal.hpp"
#include "printk.h"
#include "ctype.h"
#include "math.h"
#include "mm.h"

#define PRINTK_SERIAL 1

#if PRINTK_SERIAL
#define kputc(ch) (serial_write(ch))
#else
#define kputc(ch) (terminal::putc(ch))
#endif

static size_t kputs(const char* str) {
	size_t i;
	for (i = 0; str[i]; ++i) kputc(str[i]);
	return i;
}

static char* strnrvs(char* buf, size_t num) {
	size_t start = 0, end = num - 1;
	while (start < end) {
		const char tmp = buf[start];
		buf[start] = buf[end];
		buf[end] = tmp;
		++start;
		--end;
	}
	return buf;
}
static bool uppercase = 0;
static char* itoa(int num, char* str, int base) {
	if (!num) return str[0] = '0', str[1] = '\0', str;
	const char a = uppercase ? 'A' : 'a';
	size_t i = 0;
	bool negative = false;
	if (num < 0 && base == 10) negative = true, num = -num;
	while (num) {
		const int8_t rem = num % base;
		str[i++] = rem > 9 ? rem - 10 + a : rem + '0';
		num /= base;
	}
	if (negative) str[i++] = '-';
	str[i] = '\0';
	return strnrvs(str, i);
}
static char* utoa(unsigned num, char* str, int base, char fill, size_t width) {
	if (!num) return str[0] = '0', str[1] = '\0', str;
	const char a = uppercase ? 'A' : 'a';
	size_t i = 0;
	while (num) {
		const uint8_t rem = num % base;
		str[i++] = rem > 9 ? rem - 10 + a : rem + '0';
		num /= base;
	}
	while (i < width) str[i++] = fill;
	str[i] = '\0';
	return strnrvs(str, i);
}
static char* ptoa(const void* ptr, char* str) {
	const char a = uppercase ? 'A' : 'a';
	const size_t num_digits = sizeof(void*) * 2;
	size_t mask = 0xf;
	size_t i = 0;

	while (i < num_digits) {
		const uint8_t tmp = (((size_t)ptr) & mask) >> (i * 4);
		str[i] = tmp > 9 ? tmp - 10 + a : tmp + '0';
		mask <<= 4;
		++i;
	}
	str[i] = '\0';

	return strnrvs(str, i);
}
static char* ftoa(double val, char* str, int base, int prec, char fill, size_t width) {
	const char a = uppercase ? 'A' : 'a';
	size_t i = 0;
	bool negative = false;
	if (val == 0.0) {
		while ((i + 1) < width) str[i++] = fill;
		str[i++] = '0';
		if (prec) {
			str[i++] = '.';
			str[i++] = '0';
			for (int j = 1; j < prec; ++j) str[i++] = '0';
		}
		str[i] = '\0';
		return str;
	}
	else if (isnan(val)) {
		str[i++] = 'n';
		str[i++] = 'a';
		str[i++] = 'n';
		str[i] = '\0';
		return str;
	}
	else if (isinf(val)) {
		str[i++] = 'i';
		str[i++] = 'n';
		str[i++] = 'f';
		str[i] = '\0';
		return str;
	}
	if (signbit(val)) negative = true, val = -val;
	long ival = (long)val;
	val -= ival;

	if (ival) {
		while (ival) {
			const int rem = ival % base;
			str[i++] = rem > 9 ? rem - 10 + a : rem + '0';
			ival /= base;
		}
	}
	else str[i++] = '0';
	while (i < width) str[i++] = fill;
	if (negative) str[i++] = '-';
	strnrvs(str, i);
	if (prec > 0) {
		str[i++] = '.';
		for (int j = 0; j < prec; ++j) {
			const int rem = val *= base;
			val -= rem;
			str[i++] = rem > 9 ? rem - 10 + a : rem + '0';
		}
	}
	str[i] = '\0';
	return str;
}

static int parse_int(const char* format, size_t& i) {
	int num = 0;
	while (isdigit(format[i])) num = num * 10 + (format[i++] - '0');
	return num;
}
extern "C"
int vprintk(const char* format, va_list ap) {
	const uint8_t restore_color = terminal::get_color();
	char buffer[200];
	int written = 0;
	int flags, width, prec, tmp = 0, tmp2;
	for (size_t i = 0; format[i]; ++i) {
		char ch = format[i];
		if (ch == '%') {
			width = flags = 0, prec = 6;
			ch = format[++i];
			if (ch == '-') flags = 1, ch = format[++i];
			else if (ch == '+') flags = 2, ch = format[++i];
			else if (ch == ' ') flags = 3, ch = format[++i];
			else if (ch == '#') flags = 4, ch = format[++i];
			else if (ch == '0') flags = 5, ch = format[++i];

			if (ch == '*') width = va_arg(ap, int), ch = format[++i];
			else if (isdigit(ch)) width = parse_int(format, i), ch = format[i];

			if (ch == '.') {
				ch = format[++i];
				if (ch == '*') prec = va_arg(ap, int), ch = format[++i];
				else if (isdigit(ch)) prec = parse_int(format, i), ch = format[i];
				else return 0;
				if (prec > 50) prec = 50;
			}


			switch (ch) {
			case '%': kputc('%'), ++written; break;
			case 'c': kputc(va_arg(ap, int)), ++written; break;
			case 's': written += kputs(va_arg(ap, const char*)); break;
			case 'i':
			case 'd':
				itoa(va_arg(ap, int), buffer, 10);
				written += kputs(buffer);
				break;
			case 'x':
				tmp2 = va_arg(ap, unsigned);
				if (flags == 1) tmp = ' ';
				else if (flags == 5) tmp = '0';
				else if (flags == 4 && tmp2) kputs("0x");
				uppercase = 0;
				utoa(tmp2, buffer, 16, tmp, width);
				written += kputs(buffer);
				break;
			case 'X':
				tmp2 = va_arg(ap, unsigned);
				if (flags == 1) tmp = ' ';
				else if (flags == 5) tmp = '0';
				else if (flags == 4 && tmp2) kputs("0X");
				uppercase = 1;
				utoa(tmp2, buffer, 16, tmp, width);
				written += kputs(buffer);
				break;
			case 'o':
				tmp2 = va_arg(ap, unsigned);
				if (flags == 1) tmp = ' ';
				else if (flags == 5) tmp = '0';
				else if (flags == 4 && tmp2) kputc('0');
				utoa(tmp2, buffer, 8, tmp, width);
				written += kputs(buffer);
				break;
			case 'p':
				uppercase = 0;
				ptoa(va_arg(ap, const void*), buffer);
				written += kputs(buffer);
				break;
			case 'P':
				uppercase = 1;
				ptoa(va_arg(ap, const void*), buffer);
				written += kputs(buffer);
				break;
			case 'f':
				if (flags == 1) tmp = ' ';
				else if (flags == 5) tmp = '0';
				ftoa(va_arg(ap, double), buffer, 10, prec, tmp, width);
				written += kputs(buffer);
				break;
			case 'n':
				*va_arg(ap, int*) = written;
				break;
			case '$': {
				auto func = terminal::set_fgcolor;
				bool bg = 0;
				ch = format[++i];
				if (ch == '!') ch = format[++i], bg = 1, func = terminal::set_bgcolor;
				else if (toupper(ch) == 'R') {
					terminal::set_color(restore_color);
					break;
				}
				if (isdigit(ch)) func((terminal::VGA_Color)(ch - '0'));
				else if (ch >= 'A' && ch <= 'F')
					func((terminal::VGA_Color)(ch - 'A' + 10));
				else if (ch >= 'a' && ch <= 'f')
					func((terminal::VGA_Color)(ch - 'a' + 10));
				else kputc('%'), kputc('$'), (bg ? kputc('!') : (void)0), kputc(ch);
				break;
			}
			default:
				kputs("error: invalid printk format! char = '");
				kputc(ch);
				kputs("'");
				break;
			}
		}
		else kputc(format[i]), ++written;
	}
	terminal::set_color(restore_color);
	return written;
}
extern "C"
int printk(const char* format, ...) {
	va_list ap;
	va_start(ap, format);
	const int n = vprintk(format, ap);
	va_end(ap);
	return n;
}
void panic(const char* msg, ...) {
	va_list ap;
	disable_interrupts();
	terminal::set_fgcolor(terminal::RED);
	kputs("*** PANIC ***: ");
	va_start(ap, msg);
	vprintk(msg, ap);
	va_end(ap);
	kputc('\n');
	while (1);
}

extern enum LogLevel minimal_loglevel;
static terminal::VGA_Color logcolors[] = {
	terminal::DARK_GREEN,	// DEBUG
	terminal::BLUE,			// INFO
	terminal::YELLOW,		// WARNING
	terminal::RED,			// ERROR
	terminal::DARK_RED,		// CRITICAL
};
static const char* logstr[] = {
	"DEBUG",
	"INFO",
	"WARNING",
	"ERROR",
	"CRITICAL",
};
void log(enum LogLevel ll, const char* format, ...) {
	if (ll < minimal_loglevel) return;
	const uint8_t restore_color = terminal::get_color();
	va_list ap;
	terminal::set_bgcolor(terminal::BLACK);
	terminal::set_fgcolor(terminal::GRAY);
	kputc('[');
	terminal::set_fgcolor(logcolors[(int)ll]);
	kputs(logstr[(int)ll]);
	terminal::set_fgcolor(terminal::GRAY);
	kputc(']');
	kputc(' ');
	va_start(ap, format);
	vprintk(format, ap);
	va_end(ap);
	kputc('\n');
	terminal::set_color(restore_color);
}