#include "arch/i386/port.h"
#include "arch/i386/vga.h"
#include "string.h"
#include "ascii.h"

#define PORT_MISC		0x03c2
#define PORT_CRTC_IDX	0x03d4
#define PORT_CRTC_DATA	0x03d5
#define PORT_SEQ_IDX	0x03c4
#define PORT_SEQ_DATA	0x03c5
#define PORT_GC_IDX		0x03ce
#define PORT_GC_DATA	0x03cf
#define PORT_AC_IDX		0x03c0
#define PORT_AC_READ	0x03c1
#define PORT_AC_WRITE	0x03c0
#define PORT_AC_RESET	0x03da

static uint8_t mode320x200x8[] = {
/* MISC */
	0x63,
/* SEQ */
	0x03, 0x01, 0x0F, 0x00, 0x0E,
/* CRTC */
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
	0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x9C, 0x0E, 0x8F, 0x28,	0x40, 0x96, 0xB9, 0xA3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x41, 0x00, 0x0F, 0x00,	0x00
};

static struct {
	enum VGA_Mode mode;
	uint16_t width;
	uint16_t height;
	uint16_t depth;
	uint16_t pitch;
	uint32_t bufsize;
} curmode;

static uint8_t* hbuf;
static uint8_t buffer[320 * 200];
static bool buffering;

int vga_width(void) { return curmode.width; }
int vga_height(void) { return curmode.height; }
void vga_double_buffer(bool val) { buffering = val; }
enum VGA_Mode vga_get_mode(void) { return curmode.mode; }

inline static uint8_t* getbuf(void) {
	asm("cli");
	outb(PORT_GC_IDX, 0x06);
	const uint8_t segnum = ((inb(PORT_GC_DATA) >> 2) & 3);
	asm("sti");
	switch (segnum) {
	case 1: return (uint8_t*)0xa0000;
	case 2: return (uint8_t*)0xb0000;
	case 3: return (uint8_t*)0xb8000;
	default:return (uint8_t*)0x00000;	 
	}
}
static void write_regs(uint8_t* regs) {
	outb(PORT_MISC, *regs++);
	for (uint8_t i = 0; i < 5; ++i) {
		outb(PORT_SEQ_IDX, i);
		outb(PORT_SEQ_DATA, *regs++);
	}
	outb(PORT_CRTC_IDX, 0x03);
	outb(PORT_CRTC_DATA, inb(PORT_CRTC_DATA) | 0x80);
	outb(PORT_CRTC_IDX, 0x11);
	outb(PORT_CRTC_DATA, inb(PORT_CRTC_DATA) & ~0x80);

	regs[0x03] |= 0x80;
	regs[0x11] &= ~0x80;

	for (uint8_t i = 0; i < 25; ++i) {
		outb(PORT_CRTC_IDX, i);
		outb(PORT_CRTC_DATA, *regs++);
	}
	for (uint8_t i = 0; i < 9; ++i) {
		outb(PORT_GC_IDX, i);
		outb(PORT_GC_DATA, *regs++);
	}
	for (uint8_t i = 0; i < 21; ++i) {
		inb(PORT_AC_RESET);
		outb(PORT_AC_IDX, i);
		outb(PORT_AC_WRITE, *regs++);
	}
	inb(PORT_AC_RESET);
	outb(PORT_AC_IDX, 0x20);
}
static void set_curmode(enum VGA_Mode m) {
	curmode.mode = m;
	curmode.width = VGA_MODE_WIDTH(m);
	curmode.height = VGA_MODE_HEIGHT(m);
	curmode.depth = VGA_MODE_DEPTH(m) / 8;
	curmode.pitch = curmode.width * curmode.depth;
	curmode.bufsize = curmode.pitch * curmode.height;
	hbuf = getbuf();	
}

inline static size_t compute_scraddr(uint16_t x, uint16_t y) {
	return y * curmode.pitch + x * curmode.depth;
}

int vga_init(void) {
	set_curmode(VGA_80x25_TM);
	hbuf = (uint8_t*)0xb8000;
	memset(buffer, 0, sizeof(buffer));
	buffering = false;
	return 0;
}
bool vga_set_mode(enum VGA_Mode m) {
	switch (m) {
	case VGA_320x200x8:
		write_regs(mode320x200x8);
		break;
	default: return false;
	}
	set_curmode(m);
	return true;
}
bool vga_text_mode(void) {
	return curmode.depth == 1;
}

void vga_clear(void) {
	void* buf = buffering ? buffer : ((void*)getbuf());
	memset(buf, 0, curmode.bufsize);
}
void vgc_clear_direct(void) {
	memset(hbuf, 0, curmode.bufsize);
}
bool vga_draw(uint16_t x, uint16_t y, uint8_t cc) {
	if (x >= curmode.width || y >= curmode.height) return false;
	const size_t pos = y * curmode.pitch + (x * curmode.depth);
	if (buffering) buffer[pos] = cc;
	else hbuf[pos] = cc;
	return true;
}
bool vga_draw_direct(uint16_t x, uint16_t y, uint8_t cc) {
	if (x >= curmode.width || y >= curmode.height) return false;
	const size_t pos = y * curmode.pitch + (x * curmode.depth);
	hbuf[pos] = cc;
	return true;
}
void vga_render(void) {
	if (buffering) memcpy((void*)hbuf, buffer, curmode.bufsize);
}
void vga_blit_buf(uint8_t* buf, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const void* pixels) {
	if (!pixels || x >= curmode.width || y >= curmode.height) return;
	const uint16_t pp = w;
	if (x + w >= curmode.width) w = curmode.width - x;
	if (y + h >= curmode.height) h = curmode.height - y;
	const uint8_t* const px = (const uint8_t*)pixels;
	for (uint16_t y0 = 0; y0 < h; ++y0) {
		for (uint16_t x0 = 0; x0 < w; ++x0) {
			buf[(y0 + y) * curmode.width + (x0 + x) * curmode.depth] = px[y0 * pp + x0];
		}
	}
}
void vga_blit(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const void* pixels) {
	vga_blit_buf(buffering ? buffer : hbuf, x, y, w, h, pixels);
}
void vga_blit_direct(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const void* pixels) {
	vga_blit_buf(hbuf, x, y, w, h, pixels);
}
void ascii_blit_buf(uint8_t* buf, uint16_t x, uint16_t y, unsigned char ch, const font_t* font, uint8_t fgc, uint8_t bgc) {
	if (!font || (x + font->width) >= curmode.width || (y + font->height) >= curmode.height) return;
	const uint8_t w = font->width;
	const uint8_t h = font->height;
	const uint8_t xc = ch % 16;
	const uint8_t yc = ch / 16;
	for (uint16_t y0 = 0; y0 < h; ++y0) {
		for (uint16_t x0 = 0; x0 < w; ++x0) {
			const size_t addr = (yc * h + y0) * (w * 16) + (xc * w + x0);
			const uint8_t byte = font->bitmap[addr / 8 + 1];
			const bool val = byte & (0x80 >> (addr & 7));
			const size_t scraddr = compute_scraddr(x0 + x, y0 + y);
			buf[scraddr] = val ? fgc : bgc;
		}
	}
}
void ascii_blit_direct(uint16_t x, uint16_t y, unsigned char ch, const font_t* font, uint8_t fgc, uint8_t bgc) {
	ascii_blit_buf(hbuf, x, y, ch, font, fgc, bgc);
}
void ascii_blit(uint16_t x, uint16_t y, unsigned char ch, const font_t* font, uint8_t fgc, uint8_t bgc) {
	ascii_blit_buf(buffering ? buffer : hbuf, x, y, ch, font, fgc, bgc);
}

void vga_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t cc) {
	vga_fill_rect_buf(buffering ? buffer : hbuf, x, y, w, h, cc);
}
void vga_fill_rect_direct(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t cc) {
	vga_fill_rect_buf(hbuf, x, y, w, h, cc);
}
void vga_fill_rect_buf(uint8_t* buf, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t cc) {
	for (uint16_t y0 = 0; y0 < h; ++y0) {
		for (uint16_t x0 = 0; x0 < w; ++x0) {
			buf[compute_scraddr(x0 + x, y0 + y)] = cc;
		}
	}
}
static uint8_t read_px(uint16_t x, uint16_t y) {
	return buffer[y * curmode.pitch + x * curmode.depth];
}
void vga_blit_transparent(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const void* pixels) {
	const uint16_t p = w;
	if (x + w >= curmode.width) w = curmode.width - x;
	if (y + h >= curmode.height) h = curmode.height - y;
	uint8_t* buf = buffering ? buffer : hbuf;
	const uint8_t* px = (const uint8_t*)pixels;
	for (uint16_t y0 = 0; y0 < h; ++y0) {
		for (uint16_t x0 = 0; x0 < w; ++x0) {
			const uint8_t val = px[y0 * p + x0];
			if (val != 0xff) buf[compute_scraddr(x0 + x, y0 + y)] = val;
		}
	}
}
void vga_blit_sprite(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t fgc, const void* pixels) {
	const uint16_t p = w;
	if (x + w >= curmode.width) w = curmode.width - x;
	if (y + h >= curmode.height) h = curmode.height - y;
	const uint8_t* px = (const uint8_t*)pixels;
	uint8_t* buf = buffering ? buffer : hbuf;
	for (uint16_t y0 = 0; y0 < h; ++y0) {
		for (uint16_t x0 = 0; x0 < w; ++x0) {
			const size_t saddr = y0 * p + x0;
			const bool val = px[saddr / 8] & (0x80 >> (saddr & 7));
			if (val) buf[compute_scraddr(x0 + x, y0 + y)] = fgc;			
		}
	}

}