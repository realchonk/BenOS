#include "arch/i386/port.h"
#include "terminal.hpp"
#include "driver.hpp"
#include "printk.h"

namespace io {
	static void kbc_send_cmd(uint8_t cmd) {
		while (inb(0x64) & 0x02);
		outb(0x64, cmd);
	}
	static void mouse_send_cmd(uint8_t cmd) {
		kbc_send_cmd(0xd4);
		outb(0x60, cmd);
	}
	static void write_data(uint8_t data) {
		while (inb(0x64) & 0x02);
		outb(0x60, data);
	}
	static uint8_t read_data(void) {
		while (!(inb(0x64) & 0x01));
		return inb(0x60);
	}
	MouseDriver::MouseDriver() noexcept
		: InterruptHandler(0x2c), Driver(DriverType::MOUSE) {}
	bool MouseDriver::setup() noexcept {
		enabled = reset() != -1;
		registerSelf();
		if (!enabled) log(WARNING, "PS/2 Mouse is not supported");
		return true;
	}
	int MouseDriver::reset() noexcept {
		cycle = buttons = 0;
		short timeout = 5000;
		while (inb(0x64) & 0x01) {
			inb(0x60);
			if (timeout == 0) return -1;
			else --timeout;
		}

		kbc_send_cmd(0xa8);
		kbc_send_cmd(0x20);
		uint8_t status = read_data() | 2;
		kbc_send_cmd(0x60);
		write_data(status);

		mouse_send_cmd(0xf6);
		mouse_send_cmd(0xf4);

		posx = terminal::get_width() / 2;
		posy = terminal::get_height() / 2;
		xscale = yscale = 0.15f;
		log(DEBUG, "Mouse initialized");
		return 0;
	}
	void MouseDriver::handleInterrupt() noexcept {
		if (!enabled || !(inb(0x64) & 0x20)) return;
		buffer[cycle++] = inb(0x60);
		if (cycle == 1 && !(buffer[0] & 0x08)) cycle = 0;
		else if (cycle == 3) {
			cycle = 0;
			const float dx = (float)buffer[1] * xscale;
			const float dy = (float)buffer[2] * -yscale;
			posx += dx;
			posy += dy;
			if (posx < 0) posx = 0;
			else if (posx >= terminal::get_width()) posx = terminal::get_width() - 1;
			if (posy < 0) posy = 0;
			else if (posy >= terminal::get_height()) posy = terminal::get_height() - 1;
			if (on_mousemove) on_mousemove(posx, posy, dx, dy);
		}
		for (uint8_t i = 0; i < 3; ++i) {
			const uint8_t mask = 1 << i;
			if ((buffer[0] & mask) != (buttons & mask)) {
				if (on_mousebutton) on_mousebutton(posx, posy, i, buffer[0] & mask);
			}
		}
		buttons = buffer[0];
	}
}