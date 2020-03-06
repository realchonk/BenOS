#include <stdint.h>
#include "arch/i386/port.h"
#include "terminal.hpp"
#include "string.h"

#define WIDTH	80
#define HEIGHT	25

namespace terminal {
	static uint16_t* vidmem = (uint16_t*)0xb8000;
	static int x = 0, y = 0, tabsize = 4;
	static uint8_t color = 0x07;
	static bool autocursor = true;

	void putraw(int x, int y, char ch, uint8_t color) {
		vidmem[y * WIDTH + x] = (color << 8) | ch;
	}
	inline static void scroll_up() {
		for (int8_t i = 0; i < 24; ++i) {
			memcpy(&vidmem[i * WIDTH], &vidmem[(i + 1) * WIDTH], WIDTH * 2);
		}
		for (int8_t i = 0; i < WIDTH; ++i) {
			putraw(i, HEIGHT - 1, ' ', color);
		}
		y = HEIGHT - 1;
	}

	void putc(char ch) {
		int tmp;
		switch (ch) {
		case '\n':
			for (; x < WIDTH; ++x)
				putraw(x, y, ' ', color);
			break;
		case '\t':
			tmp = tabsize - (x % tabsize);
			for (int i = 0; i < tmp; ++i)
				putraw(x + i, y, ' ', color);
			x += tmp;
			break;
		case '\r': x = 0; break;
		case '\b':
			if (x > 0) --x;
			else if (x == 0 && y > 0) --y, x = 79;
			putraw(x, y, ' ', color);
			break;
		default:
			putraw(x++, y, ch, color);
			break;
		}
		if (x >= WIDTH) x = 0, ++y;
		if (y >= HEIGHT) scroll_up();
		if (autocursor) set_cursor(x, y);
	}

	int puts(const char* str) {
		size_t i;
		for (i = 0; str[i]; ++i) putc(str[i]);
		return (int)i;
	}
	void set_fgcolor(enum VGA_Color c) {
		color = (color & 0xf0) | c;
	}
	void set_bgcolor(enum VGA_Color c) {
		color = (color & 0x0f) | (c << 4);
	}
	uint8_t get_color(void) {
		return color;
	}
	void set_color(uint8_t c) {
		color = c;
	}
	void clear() {
		x = y = 0;
		for (size_t i = 0; i < (WIDTH * HEIGHT); ++i) vidmem[i] = (color << 8) | ' ';
	}
	void auto_cursor(bool b) { autocursor = b;}
	void set_cursor(int x, int y) {
		if (x < 0 || y < 0) return;
		uint16_t tmp = y * 80 + x;
		outb(0x3d4, 14);
		outb(0x3d5, tmp >> 8);
		outb(0x3d4, 15);
		outb(0x3d5, tmp);
	}
	void set_pos(int dx, int dy) {
		if (dx < 0) dx = WIDTH - 1, --dy;
		if (dy < 0) dy = 0;
		if (dx >= WIDTH) dx = 0, ++dy;
		if (dy >= HEIGHT) dy = HEIGHT - 1;
		x = dx;
		y = dy;
		if (autocursor) set_cursor(dx, dy);
	}
	int get_x() { return x; }
	int get_y() { return y; }
	int get_width() { return WIDTH; }
	int get_height() { return HEIGHT; }
	uint8_t read_color(int x, int y) {
		return vidmem[y * WIDTH + x] >> 8;
	}
	char read_char(int x, int y) {
		return vidmem[y * WIDTH + x] & 0xff;
	}
	void enable_cursor(void) {
		outb(0x3d4, 0x0a);
		outb(0x3d5, (inb(0x3d5) & 0xc0) | 13);

		outb(0x3d4, 0x0b);
		outb(0x3d5, (inb(0x3d5) & 0xe0) | 14);
	}
	void disable_cursor(void);
}