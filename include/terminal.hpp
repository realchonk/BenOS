#ifndef FILE_TERMINAL_HPP
#define FILE_TERMINAL_HPP
#include <stdint.h>

namespace terminal {
	enum VGA_Color {
		BLACK		= 0x0,
		DARK_BLUE	= 0x1,
		DARK_GREEN	= 0x2,
		DARK_CYAN	= 0x3,
		DARK_RED	= 0x4,
		DARK_PURPLE	= 0x5,
		DARK_YELLOW	= 0x6,
		GRAY 		= 0x7,
		DARK_GRAY	= 0x8,
		BLUE 		= 0x9,
		GREEN 		= 0xa,
		CYAN 		= 0xb,
		RED 		= 0xc,
		PURPLE 		= 0xd,
		YELLOW 		= 0xe,
		WHITE		= 0xf,
	};
	void putraw(int x, int y, char ch, uint8_t color);
	void putc(char ch);
	int puts(const char* str);
	void set_fgcolor(enum VGA_Color color);
	void set_bgcolor(enum VGA_Color color);
	uint8_t get_color(void);
	void set_color(uint8_t c);
	void clear();
	void set_pos(int x, int y);
	int get_x();
	int get_y();
	int get_width();
	int get_height();
	uint8_t read_color(int x, int y);
	char read_char(int x, int y);

	void auto_cursor(bool b);
	void set_cursor(int x, int y);
	void enable_cursor(void);
	void disable_cursor(void);
}


#endif /* FILE_TERMINAL_HPP */