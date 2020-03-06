#include "arch/i386/drivers.hpp"
#include "arch/i386/port.h"
#include "printk.h"


extern "C"
void on_key(uint8_t scancode, int vkey, uint16_t mod, bool pressed);

namespace io {
	static const int keymap_de_DE[16][256] = {
		// altgr ctrl shift
		{	// 0000
			  0,   0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',   0,   0,'\b','\t', // 0x00
			'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p',   0, '+','\n',   0, 'a', 's', // 0x10
			'd', 'f', 'g', 'h', 'j', 'k', 'l',   0,   0,   0,   0, '#', 'y', 'x', 'c', 'v', // 0x20
			'b', 'n', 'm', ',', '.', '-',   0,   0,   0, ' ',   0,   0,   0,   0,   0,   0, // 0x30
		},
		{	// 0001 
			  0,   0, '!', '"',   0, '$', '%', '&', '/', '(', ')', '=', '?',   0,'\b','\t', // 0x00
			'Q', 'W', 'E', 'R', 'T', 'Z', 'U', 'I', 'O', 'P',   0, '*','\n',   0, 'A', 'S', // 0x10
			'D', 'F', 'G', 'H', 'J', 'K', 'L',   0,   0,   0,   0,'\'', 'Y', 'X', 'C', 'V', // 0x20
			'B', 'N', 'M', ';', ':', '_',   0,   0,   0, ' ',   0,   0,   0,   0,   0,   0, // 0x30
		},
		{	// 0010
		},
		{	// 0011
		},
		{	// 0100
			 0,    0,   0,   0,   0,   0,   0,   0, '{', '[', ']', '}','\\',   0,   0,'\t', // 0x00
			'@',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, '~','\n',   0,   0,   0, // 0x10
		},
		{	// 0101
		},
		{	// 0110
		},
		{	// 0111
		},
		{	// 1000
		},
	};
	typedef const int mapper_t[256];
	static const mapper_t* mapper = keymap_de_DE;
	static bool shift = false, ctrl = false, shift_pressed = false, alt_pressed, altgr_pressed = false;
	static bool received0x60 = false;

	KeyboardDriver::KeyboardDriver() noexcept
		: InterruptHandler(0x21) {}
	bool KeyboardDriver::setup() noexcept {
		while (inb(0x64) & 0x1) inb(0x60);
		outb(0x64, 0xae);
		outb(0x64, 0x20);
		const uint8_t status = (inb(0x60) | 1) & ~0x10;
		outb(0x64, 0x60);
		outb(0x60, status);
		outb(0x60, 0xf4);
		registerSelf();
		log(DEBUG, "Keyboard initialized");
		return true;
	}
	int KeyboardDriver::reset() noexcept {
		while (inb(0x64) & 2);
		outb(0x60, 0xff);

		while (inb(0x64) & 0x1) inb(0x60);
		outb(0x64, 0xae);
		outb(0x64, 0x20);
		const uint8_t status = (inb(0x60) | 1) & ~0x10;
		outb(0x64, 0x60);
		outb(0x60, status);
		outb(0x60, 0xf4);
		log(DEBUG, "Keyboard initialized");
		return 0;
	}
	static bool check_specialkey(int scancode, uint8_t data, bool pressed) {
		if (data == 0xe0) { received0x60 = true; return true; }
		switch (scancode) {
		case 0x1d: ctrl = pressed; return true;
		case 0x2a: shift_pressed = pressed; return true;
		case 0x3a: if (!pressed) shift = !shift; return true;
		case 0xb8: altgr_pressed = pressed; return true;
		case 0x38: alt_pressed = pressed; return true;
		default: return false;
		}
	}
	void KeyboardDriver::handleInterrupt() noexcept {
		const uint8_t data = inb(0x60);
		if (data == 0xfa || data == 0xc5 || data == 0x45) return;
		const bool pressed = !(data & 0x80);
		int scancode = data & 0x7f;


		if (received0x60) {
			scancode |= 0x80;
			received0x60 = false;
		}
		if (check_specialkey(scancode, data, pressed)) {
			on_key(scancode, 0, 0, pressed);
			return;
		}

		const int mod = (alt_pressed << 3) | (altgr_pressed << 2) | (ctrl << 1) | (shift ^ shift_pressed);
		const int ch = mapper[mod][scancode];

		on_key(scancode, ch, mod, pressed);
	}

}