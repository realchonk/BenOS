#ifndef FILE_ARCH_VGA_H
#define FILE_ARCH_VGA_H
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#else
#define constexpr inline
#endif

#define MAKE_VGA_MODE(w, h, depth) ((((depth & 0xff) << 24) | ((w & 0xfff) << 12) | (h & 0xfff)))
#define VGA_MODE_WIDTH(m)	(((uint32_t)m >> 12) & 0xfff)
#define VGA_MODE_HEIGHT(m)	((uint32_t)m & 0xfff)
#define VGA_MODE_DEPTH(m)	(((uint32_t)m >> 24) & 0xff)
enum VGA_Mode {
	VGA_80x25_TM = MAKE_VGA_MODE(80, 25, 1),
	VGA_320x200x8 = MAKE_VGA_MODE(320, 200, 8),
};

constexpr static uint8_t vga_compress_color(uint8_t r, uint8_t g, uint8_t b) {
	// rrr ggg bb
	return ((r & 0x7) << 5) | ((g & 0x7) << 2) | (b & 0x3);
}
int vga_init(void);
bool vga_set_mode(enum VGA_Mode mode);
enum VGA_Mode vga_get_mode(void);
bool vga_draw(uint16_t x, uint16_t y, uint8_t cc);
bool vga_draw_direct(uint16_t x, uint16_t y, uint8_t cc);
void vga_clear(void);
void vgc_clear_direct(void);
void vga_render();
int vga_width(void);
int vga_height(void);
void vga_double_buffer(bool val);
bool vga_text_mode(void);

void vga_blit_buf(uint8_t* buf, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const void* pixels);
void vga_blit(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const void* pixels);
void vga_blit_direct(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const void* pixels);

void vga_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t cc);
void vga_fill_rect_direct(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t cc);
void vga_fill_rect_buf(uint8_t* buf, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t cc);


#ifdef __cplusplus
}
#else
#undef constexpr
#endif

#endif /* FILE_ARCH_VGA_H */