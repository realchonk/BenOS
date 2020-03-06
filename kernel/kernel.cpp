#include <stddef.h>
#include "arch/i386/multiboot.h"
#include "arch/i386/drivers.hpp"
#include "arch/i386/port.h"
#include "terminal.hpp"
#include "driver.hpp"
#include "printk.h"
#include "heap.hpp"
#include "mm.h"

extern "C" {
void cpu_reset(void);
void abort();
enum LogLevel minimal_loglevel = DEBUG;

void on_key(uint8_t scancode, int vkey, uint16_t mod, bool pressed) {
	(void)scancode;
	(void)mod;
	//printk("0x%x\n", scancode);
	if (!pressed) return;

	switch (scancode) {
	case 0xc8: terminal::set_pos(terminal::get_x(), terminal::get_y() - 1); return;
	case 0xd0: terminal::set_pos(terminal::get_x(), terminal::get_y() + 1); return;
	case 0xcb:
		if (mod & 0x2) terminal::set_pos(1, terminal::get_y());
		terminal::set_pos(terminal::get_x() - 1, terminal::get_y()); return;
	case 0xcd: terminal::set_pos(terminal::get_x() + 1, terminal::get_y()); return;
	case 0xd3:
		// ctrl and alt
		if ((mod & 0x2)/* && (mod & 0x8)*/) cpu_reset();
		else terminal::putraw(terminal::get_x(), terminal::get_y(), ' ', terminal::get_color());
		return;
	}

	if (vkey) printk("%c", vkey);
	else if (mod & 0x08) {
		auto f = mod & 0x1 ? terminal::set_bgcolor : terminal::set_fgcolor;
		switch (scancode) {
		case 0x2:
			f((terminal::VGA_Color)1);
			break;
		case 0x3:
			f((terminal::VGA_Color)2);
			break;
		case 0x4:
			f((terminal::VGA_Color)3);
			break;
		case 0x5:
			f((terminal::VGA_Color)4);
			break;
		case 0x6:
			f((terminal::VGA_Color)5);
			break;
		case 0x7:
			f((terminal::VGA_Color)6);
			break;
		case 0x8:
			f((terminal::VGA_Color)7);
			break;
		case 0x9:
			f((terminal::VGA_Color)8);
			break;
		case 0xa:
			f((terminal::VGA_Color)9);
			break;
		case 0xb:
			f((terminal::VGA_Color)0);
			break;
		case 0x1e:
			f((terminal::VGA_Color)10);
			break;
		case 0x30:
			f((terminal::VGA_Color)11);
			break;
		case 0x2e:
			f((terminal::VGA_Color)12);
			break;
		case 0x20:
			f((terminal::VGA_Color)13);
			break;
		case 0x12:
			f((terminal::VGA_Color)14);
			break;
		case 0x21:
			f((terminal::VGA_Color)15);
			break;
		}
	}
	//else printk("unknown key 0x%x\n", scancode);
}
void on_mousemove(float x, float y, float dx, float dy) {
	(void)x, (void)y;
	(void)dx, (void)dy;
}
void on_mousebutton(float x, float y, int btn, bool pressed) {
	if (pressed && btn == 0) {
		terminal::set_pos(x, y);
	}
}

extern void* heap;
mm::HeapManager* heapmgr;

}

#define PS2MOUSE 0

static void ktest();
extern "C"
void kernel_main(int aborted, const multiboot_info* mbinfo, int magicnum) {
	auto flip = [](uint8_t n) -> uint8_t {
		return (n >> 4) | ((n & 0xf) << 4);
	};

	(void)magicnum;
	terminal::enable_cursor();
	terminal::clear();
	log(INFO, "Kernel starting");
	if (aborted) { log(WARNING, "Kernel was aborted"); while(1); }
	mm_init();

	log(INFO, "Total Memory: %dMiB", (mbinfo->mem_upper >> 10) + 2);
	mm::HeapManager hmgr(&heap, (mbinfo->mem_upper << 10) - (uint32_t)&heap);
	heapmgr = &hmgr;
	log(INFO, "Available Memory for Programs: %.1fMiB", (float)hmgr.totalHeap() / 1024.f / 1024.f);
	mm::heapmgr = &hmgr;

	io::KeyboardDriver keyboard;
	io::DriverManager::add(&keyboard);
#if PS2MOUSE
	io::MouseDriver mouse;
	io::DriverManager::add(&mouse);
#endif

	io::DriverManager::setupAll();

	log(DEBUG, "Interrupts enabled");
	enable_interrupts();

	ktest();


	while(1) {
#if PS2MOUSE
		const int x = mouse.posx;
		const int y = mouse.posy;
		terminal::putraw(x, y, terminal::read_char(x, y), flip(terminal::read_color(x, y)));
		for (int i = 0; i < 10000; ++i);
		terminal::putraw(x, y, terminal::read_char(x, y), flip(terminal::read_color(x, y)));
#endif
	}
}

#include "stdlib.h"


static void ktest() {

}