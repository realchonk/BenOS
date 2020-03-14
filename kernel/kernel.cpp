#include <stddef.h>
#include "arch/i386/multiboot.h"
#include "arch/i386/serial.h"
#include "arch/i386/pci.h"
#include "arch/i386/vga.h"
#include "terminal.hpp"
#include "driver.hpp"
#include "printk.h"
#include "stdlib.h"
#include "paging.h"
#include "heap.hpp"
#include "task.h"
#include "mm.h"

extern "C" {
void cpu_reset(void);
void abort();
enum LogLevel minimal_loglevel = DEBUG;
extern void* heap;
}

#define PS2MOUSE 1

namespace basic {
	void init();
}

static constexpr auto kernel_heap_size = 4 << 20;
static void ktest();
static vmm_context* kernel_ctx;
extern "C"
void kernel_main(int aborted, const multiboot_info* mbinfo, int magicnum) {
	(void)aborted;
	(void)magicnum;

	serial_init(12);

	terminal::enable_cursor();
	terminal::clear();
	log(INFO, "Kernel starting");
	//if (aborted) log(WARNING, "Kernel was aborted");
	mm_init();

	log(INFO, "Total Memory: %$3%dMiB", (mbinfo->mem_upper >> 10) + 2);
	mm::HeapManager hmgr(&heap, kernel_heap_size);
	mm::heapmgr = &hmgr;

	io::KeyboardDriver keyboard;
	io::DriverManager::add(&keyboard);
#if PS2MOUSE
	io::MouseDriver mouse;
	io::DriverManager::add(&mouse);
#endif

	pci_init();
	io::DriverManager::setupAll();

	kernel_ctx = vmm_create_context();
	constexpr auto identity_map = 2048;
	for (size_t i = 0; i < identity_map; ++i) {
		vmm_map_page(kernel_ctx, i << 12, i << 12, false);
	}
	vmm_map_page(kernel_ctx, 2048 << 12, 2048 << 12, false);
	vmm_activate_context(kernel_ctx);
	vmm_enable();

	log(DEBUG, "Identity mapped the first %.1fMB of memory", identity_map * 4096 / 1024.f / 1024.f);
	log(DEBUG, "Kernel Heap Usage: (%dkB/%dkB)", mm::heapmgr->usedHeap() >> 10, mm::heapmgr->totalHeap() >> 10);

	taskmgr_init();

	ktest();

	log(DEBUG, "Interrupts enabled");
	enable_interrupts();

	while (1);
}

extern "C"
void no_task_handler(void) {
	log(CRITICAL, "No tasks to execute");
	abort();
}
#include "syscall.h"
#include "string.h"
#include "ascii.h"

static uint8_t cursor_bitmap[8] = {
	0b11111111,
	0b11111111,
	0b11110000,
	0b11110000,
	0b11001100,
	0b11001100,
	0b11000011,
	0b11000011,
};
static void vga_updater() {
	static uint8_t surface[80 * 50];
	static uint8_t fgc = 0xf, bgc = 0x0;
	static io::MouseDriver* mouse;
	auto on_key = [](uint8_t sc, int, uint16_t mod, bool) {
		int val = -1;
		int vkey = io::KeyboardDriver::transform(sc, 0);
		if (vkey >= '0' && vkey <= '9') val = vkey - '0';
		else if (vkey >= 'a' && vkey <= 'f') val = vkey - 'a' + 10;
		else if (sc == 1) memset(surface, bgc, sizeof(surface));

		if (val != -1) {
			if (mod & 1) bgc = val;
			else fgc = val;
		}
	};
	auto on_btn = [](float x, float y, int btn, bool) {
		surface[(int)(y / 4) * 80 + (int)(x / 4)] = btn == 0 ? fgc : bgc;
	};
	auto on_move = [](float x, float y, float, float) {
		uint8_t& b = surface[(int)(y / 4) * 80 + (int)(x / 4)];
		if (mouse->get_button(1)) b = bgc;
		if (mouse->get_button(0)) b = fgc;
	};

	vga_init();
	vga_set_mode(VGA_320x200x8);
	vga_double_buffer(true);
	io::KeyboardDriver* kb = (io::KeyboardDriver*)io::DriverManager::find(io::DriverType::KEYBOARD);
	mouse = (io::MouseDriver*)io::DriverManager::find(io::DriverType::MOUSE);
	if (kb) {
		kb->on_key = on_key;
	}
	if (mouse) {
		mouse->on_mousebutton = on_btn;
		mouse->on_mousemove = on_move;
	}

	while (1) {
		vga_clear();
		for (int y = 0; y < 50; ++y) {
			for (int x = 0; x < 80; ++x) {
				vga_fill_rect(x * 4, y * 4, 4, 4, surface[y * 80 + x]);
			}
		}
		vga_blit_sprite(mouse->posx, mouse->posy, 8, 8, fgc, cursor_bitmap);
		//ascii_blit(mouse->posx, mouse->posy, 'A', &ascii8, fgc, 0x00);
		vga_render();
	}
}

static void task1() {
	while (1) printk("a");
}
static void task2() {
	while (1) printk("b");
}

static void ktest() {
	//taskmgr_add(taskmgr_create_task(task1));
	//taskmgr_add(taskmgr_create_task(task2));
	taskmgr_add(taskmgr_create_task(vga_updater));
}	