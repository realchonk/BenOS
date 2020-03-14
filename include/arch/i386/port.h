#ifndef FILE_ARCH_PORT_H
#define FILE_ARCH_PORT_H
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

inline static void outb(uint16_t port, uint8_t data) {
	asm volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}
inline static void outb_slow(uint16_t port, uint8_t data) {
	asm volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:" : : "a"(data), "Nd"(port));
}
inline static void outw(uint16_t port, uint16_t data) {
	asm volatile("outw %0, %1" : : "a"(data), "Nd"(port));
}
inline static void outl(uint16_t port, uint32_t data) {
	asm volatile("outl %0, %1" : : "a"(data), "Nd"(port));
}

inline static uint8_t  inb(uint16_t port) {
	volatile uint8_t data;
	asm volatile("inb %1, %0" : "=a"(data) : "Nd"(port));
	return data;
}
inline static uint16_t inw(uint16_t port) {
	volatile uint16_t data;
	asm volatile("inw %1, %0" : "=a"(data) : "Nd"(port));
	return data;
}
inline static uint32_t inl(uint16_t port) {
	volatile uint32_t data;
	asm volatile("inl %1, %0" : "=a"(data) : "Nd"(port));
	return data;
}

inline static void cmos_write(bool nmi, uint8_t reg, uint8_t val) {
	outb(0x70, (nmi << 7) | (reg & 0x7f));
	outb(0x71, val);
}
inline static uint8_t cmos_read(bool nmi, uint8_t reg) {
	outb(0x70, (nmi << 7) | (reg & 0x7f));
	return inb(0x71);
}

#ifdef __cplusplus
}
#endif

#endif /* FILE_ARCH_PORT_H */