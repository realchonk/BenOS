#ifndef FILE_SERIAL_H
#define FILE_SERIAL_H
#include "arch/i386/port.h"

#define COM1 0x3f8
#define COM_CLK_RATE 115200

#ifdef __cplusplus
extern "C" {
#endif

inline static void serial_init(uint16_t divisor) {
	outb(COM1 + 1, 0x00);	// disable interrupts
	outb(COM1 + 3, 0x80);	// enable divisor
	outb(COM1 + 0, divisor & 0xff);	// set divisor (low byte)
	outb(COM1 + 1, divisor >> 8);	// set divisor (high byte)
	outb(COM1 + 3, 0x03);	// 8 bits, no parity, one stop bit
	outb(COM1 + 2, 0xc7);	// enable FIFO
	outb(COM1 + 4, 0x0b);	// enable interrupts
}
inline static bool serial_received(void) {
	return inb(COM1 + 5) & 1;
}
inline static uint8_t serial_read(void) {
	while (!serial_received());
	return inb(COM1);
}
inline static bool serial_ready(void) {
	return inb(COM1 + 5) & 0x20;
}
inline static void serial_write(uint8_t b) {
	while (!serial_ready());
	outb(COM1, b);
}

#ifdef __cplusplus
}
#endif

#endif /* FILE_SERIAL_H */