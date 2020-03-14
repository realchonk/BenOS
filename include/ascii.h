#ifndef FILE_ASCII_H
#define FILE_ASCII_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct font {
	uint8_t width;
	uint8_t height;
	const uint8_t* bitmap;
} font_t;

extern const font_t ascii8;
extern const font_t ascii12;
extern const font_t ascii16;

void ascii_blit_buf(uint8_t* buf, uint16_t x, uint16_t y, unsigned char ch, const font_t* font, uint8_t fgc, uint8_t bgc);
void ascii_blit(uint16_t x, uint16_t y, unsigned char ch, const font_t* font, uint8_t fgc, uint8_t bgc);
void ascii_blit_direct(uint16_t x, uint16_t y, unsigned char ch, const font_t* font, uint8_t fgc, uint8_t bgc);

#define ascii16_blit(x, y, ch, fgc, bgc) ascii_blit(x, y, ch, &ascii16, fgc, bgc)

#ifdef __cplusplus
}
#endif 

#endif /* FILE_ASCII_H */