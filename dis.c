/*
 * dis.c
 *
 *  Created on: 11 nov. 2020
 *      Author: andre
 */


#include <stdbool.h>

#include "ucglib_hal.h"
#include "graphics.h"

static ucg_t m_ucg;

int display_init() {

	//ucg_Init(&m_ucg, ucg_dev_ssd1331_18x96x64_univision, ucg_ext_ssd1331_18, ucg_com_stm32f1);


	// The bigger screen works so the HAL code should be fine.
	ucg_Init(&m_ucg, ucg_dev_st7735_18x128x160, ucg_ext_st7735_18, ucg_com_stm32f4);

	ucg_SetFontMode(&m_ucg, UCG_FONT_MODE_TRANSPARENT);

	// Well... it seems the display renders the speed-o-meter with an offset
	// once the rotation is enabled...
	//ucg_SetRotate180(&m_ucg);

	ucg_SetRotate90(&m_ucg);

	ucg_ClearScreen(&m_ucg);

}

//#define FB_444
#define FB_565

#ifdef FB_444
#pragma pack(push,1)
typedef struct {
	unsigned int r1 :4 __attribute__ ((packed));
	unsigned int g1 :4 __attribute__ ((packed));
	unsigned int b1 :4 __attribute__ ((packed));

	unsigned int r2 :4 __attribute__ ((packed));
	unsigned int g2 :4 __attribute__ ((packed));
	unsigned int b2 :4 __attribute__ ((packed));
} rgb444_t;

typedef union {
	uint8_t line_bytes[198];
	rgb444_t line_pixels[132 / 2];
} line444_t;

line444_t g_framebuffer444[162];
//line444_t g_framebuffer444[64]; // F103 has little ram

#pragma pack(pop)

void framebuffer_apply() {

	{
		uint8_t cmd[] = { 0x2A };
		ucg_com_SetCSLineStatus(&m_ucg, 0);
		ucg_com_SetCDLineStatus(&m_ucg, 0);
		ucg_com_SendString(&m_ucg, sizeof(cmd), cmd);

		//uint8_t data[] = { 0, 0, 0, 127 };
		uint8_t data[] = { 0, 0, 0, 131 }; // 132 pixels wide??
		ucg_com_SetCDLineStatus(&m_ucg, 1);
		ucg_com_SendString(&m_ucg, sizeof(data), data);
		ucg_com_SetCSLineStatus(&m_ucg, 1);
	}

	{
		uint8_t cmd[] = { 0x2B };
		ucg_com_SetCSLineStatus(&m_ucg, 0);
		ucg_com_SetCDLineStatus(&m_ucg, 0);
		ucg_com_SendString(&m_ucg, sizeof(cmd), cmd);

		//uint8_t data[] = { 0, 0, 0, 159 };
		uint8_t data[] = { 0, 0, 0, 161 }; //  162 pixels high ??
		ucg_com_SetCDLineStatus(&m_ucg, 1);
		ucg_com_SendString(&m_ucg, sizeof(data), data);
		ucg_com_SetCSLineStatus(&m_ucg, 1);
	}

	// page 38
	uint8_t cmd[] = { 0x2C };
	ucg_com_SetCSLineStatus(&m_ucg, 0);
	ucg_com_SetCDLineStatus(&m_ucg, 0);
	//NRFX_DELAY_US(1);
	ucg_com_SendString(&m_ucg, sizeof(cmd), cmd);

	ucg_com_SetCDLineStatus(&m_ucg, 1);
	//NRFX_DELAY_US(1);
	ucg_com_SendString(&m_ucg, sizeof(g_framebuffer444), g_framebuffer444);
	ucg_com_SetCSLineStatus(&m_ucg, 1);

	ucg_com_SetCDLineStatus(&m_ucg, 0);
}

void set_444() {
	uint8_t cmd[] = { 0x3A };
	ucg_com_SetCSLineStatus(&m_ucg, 0);
	ucg_com_SetCDLineStatus(&m_ucg, 0);
	ucg_com_SendString(&m_ucg, sizeof(cmd), cmd);

	uint8_t data[] = { 0b00111011 }; /// 444 mode
	ucg_com_SetCDLineStatus(&m_ucg, 1);
	ucg_com_SendString(&m_ucg, sizeof(data), data);
	ucg_com_SetCSLineStatus(&m_ucg, 1);
}

void set_pixel444(int x, int y, int r, int g, int b) {

	/*
	 if (x % 2) {
	 g_framebuffer444[y].line_pixels[x >> 1].r1 = r;
	 g_framebuffer444[y].line_pixels[x >> 1].b1 = b;
	 g_framebuffer444[y].line_pixels[x >> 1].g1 = g;
	 } else {
	 g_framebuffer444[y].line_pixels[x >> 1].r2 = r;
	 g_framebuffer444[y].line_pixels[x >> 1].b2 = b;
	 g_framebuffer444[y].line_pixels[x >> 1].g2 = g;
	 }
	 */
	if (x % 2) {
		g_framebuffer444[y].line_bytes[x * 3 / 2] &= 0xF0;
		g_framebuffer444[y].line_bytes[x * 3 / 2] |= r;
		g_framebuffer444[y].line_bytes[(x * 3 / 2) + 1] = g << 4 | b;
	} else {
		g_framebuffer444[y].line_bytes[x * 3 / 2] = r << 4 | g;
		g_framebuffer444[y].line_bytes[(x * 3 / 2) + 1] &= 0x0F;
		g_framebuffer444[y].line_bytes[(x * 3 / 2) + 1] |= b << 4;

	}
}

void blank_full_screen(void) {
	memset(g_framebuffer444, 0, sizeof(g_framebuffer444));
	framebuffer_apply();
}

void white_full_screen(void) {
	memset(g_framebuffer444, 0xFF, sizeof(g_framebuffer444));
	framebuffer_apply();
}

void set_pixel888(int x, int y, int r, int g, int b) {
#ifdef FB_444
	set_pixel444(x, y, r >> 4, g >> 4, b >> 4);
#elif defined FB_565
	set_pixel565(x, y, r >> 3, g >> 2, b >> 3);
#endif
}

void redfade(void) {
	static uint8_t r = 0;
	for (int r = 0; r < 16; r++) {
		for (int y = 0; y < 162; y++) {
			for (int x = 0; x < 132; x++)
				set_pixel444(x, y, r, 0x0, 0x0);
		}
		framebuffer_apply();
	}
	for (int r = 15; r; r--) {
		for (int y = 0; y < 162; y++) {
			for (int x = 0; x < 132; x++)
				set_pixel444(x, y, r, 0x0, 0x0);
		}
		framebuffer_apply();
	}
}



//---
//static_assert(sizeof(line444_t) == 198, "CHECK LINE SIZE");
#endif



#ifdef FB_565


//#define SCREEN_HEIGHT (162)
//#define SCREEN_WIDTH  (132)

#define SCREEN_HEIGHT (160)
#define SCREEN_WIDTH  (128)


#pragma pack(1)

typedef union {
	struct {
		unsigned int b : 5;
		unsigned int g : 6;
		unsigned int r : 5;
	};
	uint16_t data;
} pixel_t;

typedef struct {
	uint16_t line[SCREEN_WIDTH];
}line565_t;

line565_t g_framebuffer565[SCREEN_HEIGHT];
#pragma pack()


void set_pixel_565(int x, int y, pixel_t colour) {
	g_framebuffer565[y].line[x] = __builtin_bswap16(colour.data);
}

void set_pixel_rgb888(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
	pixel_t colour = {.r = r >> 3, .g = g >> 2, .b = b >> 3};
	g_framebuffer565[y].line[x] = __builtin_bswap16(colour.data);
}

void set_565() {
	uint8_t cmd[] = { 0x3A };
	ucg_com_SetCSLineStatus(&m_ucg, 0);
	ucg_com_SetCDLineStatus(&m_ucg, 0);
	ucg_com_SendString(&m_ucg, sizeof(cmd), cmd);

	uint8_t data[] = { 0b00111101 }; /// 565 mode
	ucg_com_SetCDLineStatus(&m_ucg, 1);
	ucg_com_SendString(&m_ucg, sizeof(data), data);
	ucg_com_SetCSLineStatus(&m_ucg, 1);
}

void framebuffer_apply() {

	{
		uint8_t cmd[] = { 0x2A };
		ucg_com_SetCSLineStatus(&m_ucg, 0);
		ucg_com_SetCDLineStatus(&m_ucg, 0);
		ucg_com_SendString(&m_ucg, sizeof(cmd), cmd);

		uint16_t data[] = { 0, __builtin_bswap16(SCREEN_WIDTH - 1)  }; // 132 pixels wide??
		ucg_com_SetCDLineStatus(&m_ucg, 1);
		ucg_com_SendString(&m_ucg, sizeof(data), data);
		ucg_com_SetCSLineStatus(&m_ucg, 1);
	}



	{
		uint8_t cmd[] = { 0x2B };
		ucg_com_SetCSLineStatus(&m_ucg, 0);
		ucg_com_SetCDLineStatus(&m_ucg, 0);
		ucg_com_SendString(&m_ucg, sizeof(cmd), cmd);

		uint16_t data[] = {0, __builtin_bswap16(SCREEN_HEIGHT - 1) };
		ucg_com_SetCDLineStatus(&m_ucg, 1);
		ucg_com_SendString(&m_ucg, sizeof(data), data);
		ucg_com_SetCSLineStatus(&m_ucg, 1);
	}

	// page 38
	uint8_t cmd[] = { 0x2C };
	ucg_com_SetCSLineStatus(&m_ucg, 0);
	ucg_com_SetCDLineStatus(&m_ucg, 0);
	//NRFX_DELAY_US(1);
	ucg_com_SendString(&m_ucg, sizeof(cmd), cmd);

	ucg_com_SetCDLineStatus(&m_ucg, 1);
	//NRFX_DELAY_US(1);
	ucg_com_SendString(&m_ucg, sizeof(g_framebuffer565), g_framebuffer565);
	ucg_com_SetCSLineStatus(&m_ucg, 1);

	ucg_com_SetCDLineStatus(&m_ucg, 0);
}

void getcolour(int y, int*r,int*g,int*b) {
	static int colour = 0;
	if (!y) colour++;
	int factor1 = 120;
	int factor2 = 160;
	int max_intensity = 0xEE;
	*r = *g = *b = 0;
	if (colour > (3*factor1)) colour = 0;
	float rColour = ((float)(colour) / (float)(factor1)  + (float)(y) / (float)(factor2));
	if (rColour > 3.0f) rColour -= 3.0f;

	if (rColour < 1.0f) {
		float rg = rColour;
		*g = (float)(max_intensity) * rg;
		*r = (float)(max_intensity) * (1.0f - rg);
	} else if (  rColour  < 2.0f) {
			float gb = rColour - 1;;
		*b = (float)(max_intensity) * gb;
		*g = (float)(max_intensity) * (1.0f - gb);
	} else if (  rColour  < 3.0f) {
		float br = rColour - 2;
		*r = (float)(max_intensity) * br;
		*b = (float)(max_intensity) * (1.0f - br);
	} else {
		*r = 255;
	}

}

void draw_background(void) {
	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		int r,g,b;
		getcolour(y,&r,&g,&b);
		for (int x = 0; x < SCREEN_WIDTH; x++) {
			set_pixel_rgb888(x, y, r, g, b);
		}
	}
}


void draw_plain_background(void) {
	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++) {
			int r,g,b;
			r = 0xCC; b=0x00; g=0x00;
			set_pixel_rgb888(x, y, r, g, b);
		}
	}
}

void draw_test() {
	static int xpos = 0;
	static int ypos = 0;
	static int xdir = 1;
	static int ydir = 1;
	const int w = 8;
	const int h = 8;

	if (xdir) {
		xpos++;
		xdir = (xpos + w < SCREEN_WIDTH);
	} else {
		xpos--;
		xdir = (xpos == 0);
	}


	if (ydir) {
		ypos++;
		ydir = (ypos + h < SCREEN_HEIGHT);
	} else {
		ypos--;
		ydir = (ypos == 0);
	}

	for ( int x = xpos; x < xpos + w; x++)
		for ( int y = ypos; y < ypos + h; y++)
			set_pixel_rgb888(x,y, 0xCC,0xCC,0);
}


#endif


void drawline(int offset_x, int y, int w, uint8_t *line, int bpp) {
	int count = 0;

	uint8_t begin_r = 0xa0, begin_g = 0xa0, begin_b = 0xa0;
	int begin_x = -1;

	for (int x = 0; x < w; x++) {
		// draw

		uint8_t r, b, g;
		switch (bpp) {
		case 2: {
			rgb565_t *pixel = line + count;
			r = pixel->r << 3;
			g = pixel->g << 2;
			b = pixel->b << 3;
		}
		break;
		case 3: {
			rgb_t *pixel = line + count;
			r = pixel->r;
			g = pixel->g;
			b = pixel->b;
		}
		break;

		case 4: {
			rgba_t *pixel = line + count;
			if (pixel->a)
				continue;
			r = pixel->r;
			g = pixel->g;
			b = pixel->b;
		}
		break;
		}
		count += bpp;

		/*
		 if (r == 0xa0 && g == 0xa0 && b == 0xa0)
		 continue;

		 ucg_SetColor(&m_ucg, 0, r, g, b);
		 ucg_DrawPixel(&m_ucg, x + offset_x, y);
		 */

		if (begin_r != r || begin_b != b || begin_g != g || x == (w - 1)) {
			if (begin_x != -1) {
				if (begin_r == 0xa0 && begin_g == 0xa0 && begin_b == 0xa0) {
					// skip transparant colour
				} else {
					/*
					 ucg_SetColor(&m_ucg, 0, begin_r, begin_g, begin_b);
					 ucg_Draw90Line(&m_ucg, begin_x + offset_x, y, x - begin_x,
					 0, 0);
					 */
					for (int sx = begin_x + offset_x;
							sx < begin_x + offset_x + (x - begin_x); sx++) {
						set_pixel_rgb888(sx, y, begin_r, begin_g, begin_b);
					}

				}
			}
			begin_r = r;
			begin_g = g;
			begin_b = b;
			begin_x = x;
		}

	}
	y++;
}

void draw_image(image_t *image, int x_offset, int y_offset) {
	uint8_t *line = image->pixel_data;
	for (int y = 0; y < image->height; y++) {
		drawline(x_offset, y + y_offset, image->width, line,
				image->bytes_per_pixel);
		line += (image->width * image->bytes_per_pixel);
	}
}

//-- Patching into ucg's font routine
void ucg_Draw90Line(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t len,
		ucg_int_t dir, ucg_int_t col_idx) {

	for (int p = 0; p < len; p++) {
		set_pixel_rgb888(x + p, y, 0xFF, 0xFF, 0xFF);
	}

}

void ucg_DrawHLine(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t len) {
	ucg_Draw90Line(ucg, x, y, len, 0, 0);
}

void print_fps(int fps) {
	ucg_SetFont(&m_ucg, ucg_font_5x8_tf);
	ucg_SetFontMode(&m_ucg, UCG_FONT_MODE_TRANSPARENT);
	char sFps[16];
	sprintf (sFps, "%3d fps", fps);
	ucg_DrawString(&m_ucg, 0, 8, 0, sFps);

}
