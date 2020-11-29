/*
 * dis.c
 *
 *  Created on: 11 nov. 2020
 *      Author: andre
 */


#include "ucglib_hal.h"

static ucg_t m_ucg;



#define SSD1331

int display_init() {

#ifdef SSD1331
	ucg_Init(&m_ucg, ucg_dev_ssd1331_18x96x64_univision, ucg_ext_ssd1331_18, ucg_com_stm32f1);
#endif

	// The bigger screen works so the HAL code should be fine.
	//ucg_Init(&m_ucg, ucg_dev_st7735_18x128x160, ucg_ext_st7735_18, ucg_com_stm32f4);

	ucg_SetFontMode(&m_ucg, UCG_FONT_MODE_TRANSPARENT);

	// Well... it seems the display renders the speed-o-meter with an offset
	// once the rotation is enabled...
	//ucg_SetRotate180(&m_ucg);

	ucg_SetRotate90(&m_ucg);

	ucg_ClearScreen(&m_ucg);

}


#ifdef SSD1331

#define SCREEN_HEIGHT (64)
#define SCREEN_WIDTH  (96)

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
	{
		// COM means column?
		//uint8_t cmd[] = { 0xA0, 0b01000000 }; // 16 bit colour mode
		//uint8_t cmd[] = { 0xA0, 0b01000010 }; // flip image horizontally
		//uint8_t cmd[] = { 0xA0, 0b01000100 }; // flip RGB to BGR
		//uint8_t cmd[] = { 0xA0, 0b01001000 }; // Not quite sure, it garbles things up, needs closer look to analyse
		//uint8_t cmd[] = { 0xA0, 0b01010000 }; // Flip Horizontally.
		//uint8_t cmd[] = { 0xA0, 0b01100000 }; // ?? COM split? (appers to mean no interlacing or something?)
		  uint8_t cmd[] = { 0xA0, 0b01110010 }; // So this one rotates 180 degrees



				ucg_com_SetCSLineStatus(&m_ucg, 0);
		ucg_com_SetCDLineStatus(&m_ucg, 0);
		ucg_com_SendString(&m_ucg, sizeof(cmd), cmd);
		ucg_com_SetCSLineStatus(&m_ucg, 1);
	}


	{
		// "Set master current attenuation factor"
		// Default value is 0x0F
		// Setting this to a lower value such as 0x04 solves the flicker
		uint8_t cmd[] = { 0x87, 0x02 };

		ucg_com_SetCSLineStatus(&m_ucg, 0);
		ucg_com_SetCDLineStatus(&m_ucg, 0);
		ucg_com_SendString(&m_ucg, sizeof(cmd), cmd);
		ucg_com_SetCSLineStatus(&m_ucg, 1);

	}


	/*
	{
		uint8_t cmd[] = { 0x8A, 0x80, 0x8B, 0x80, 0x8C, 0x80 };
		ucg_com_SetCSLineStatus(&m_ucg, 0);
		ucg_com_SetCDLineStatus(&m_ucg, 0);
		ucg_com_SendString(&m_ucg, sizeof(cmd), cmd);
		ucg_com_SetCSLineStatus(&m_ucg, 1);

	}
	*/
}

void getcolour(int y, int*r,int*g,int*b) {
	static int colour = 0;
	if (!y) colour++;
	int factor1 = 96;
	int factor2 = 64;
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

void getcolour_int(int y, int*r,int*g,int*b) {
	static int colour = 0;
	if (!y) colour++;
	int factor1 = 4096;
	int factor2 = 160;
	int max_intensity = 0xEE;
	*r = *g = *b = 0;
	if (colour > (3*factor1)) colour = 0;
	int rColour = ((1000* colour) / (float)(factor1)  + (1000*y) / (float)(factor2));
	if (rColour > 3.0f) rColour -= 3.0f;

	if (rColour < 1000) {
		int rg = rColour;
		*g = max_intensity * rg / 1000;
		*r = (max_intensity) * (1000 - rg) / 1000;
	} else if (  rColour  < 2000) {
			int gb = rColour - 1;;
		*b = (max_intensity) * gb / 1000;
		*g = (max_intensity) * (1000 - gb) / 1000;
	} else if (  rColour  < 3000) {
		int br = rColour - 2;
		*r = (max_intensity) * br / 1000;
		*b = (max_intensity) * (1000 - br) / 1000;
	} else {
		*r = 255;
	}

}

void draw_plain_background(void) {
	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++) {
			int r,g,b;
			//r = 0x00; b=0xcc; g=0xcc;
			r = 0x00; b=0x00; g=0x00;
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

	int r,g,b;

	for ( int y = ypos; y < ypos + h; y++){
		getcolour(y+1,&g,&b,&r);
		for ( int x = xpos; x < xpos + w; x++)
			set_pixel_rgb888(x,y, r,g,b);
	}

}


void framebuffer_apply() {

	{
		uint8_t cmd[] = { 0x15, 0x00, 0x5F };
		ucg_com_SetCSLineStatus(&m_ucg, 0);
		ucg_com_SetCDLineStatus(&m_ucg, 0);
		ucg_com_SendString(&m_ucg, sizeof(cmd), cmd);
		ucg_com_SetCSLineStatus(&m_ucg, 1);
	}
	{
		uint8_t cmd[] = { 0x75, 0x00, 0x3F };
		ucg_com_SetCSLineStatus(&m_ucg, 0);
		ucg_com_SetCDLineStatus(&m_ucg, 0);
		ucg_com_SendString(&m_ucg, sizeof(cmd), cmd);
		ucg_com_SetCSLineStatus(&m_ucg, 1);
	}


	{
	ucg_com_SetCSLineStatus(&m_ucg, 0);
	ucg_com_SetCDLineStatus(&m_ucg, 1);
	ucg_com_SendString(&m_ucg, sizeof(g_framebuffer565), g_framebuffer565);
	ucg_com_SetCSLineStatus(&m_ucg, 1);
	ucg_com_SetCDLineStatus(&m_ucg, 0);
	}
}
#endif

//#define FB_444

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

//line444_t g_framebuffer444[162];
line444_t g_framebuffer444[64];

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

void test_framebuffer444() {
	memset(g_framebuffer444, 0, sizeof(g_framebuffer444));

	for (int x = 10; x < 122; x++)
		set_pixel444(x, 10, 0xF, 0x0, 0x0);

	for (int x = 10; x < 122; x++)
		set_pixel444(x, 152, 0x0, 0xF, 0x0);

	for (int y = 10; y < 152; y++)
		set_pixel444(10, y, 0x0, 0x0, 0xF);

	for (int y = 10; y < 152; y++)
		set_pixel444(122, y, 0xF, 0x0, 0xF);

}

//---
/*
void framebuffer444_drawline(int offset_x, int y, int w, uint8_t *line, int bpp) {
	int count = 0;

	for (int x = 0; x < w; x++) {
		// draw

		uint8_t r, b, g;
		switch (bpp) {
		case 2: {
			rgb565_t *pixel = line + count;
			r = pixel->r >> 1;
			g = pixel->g >> 2;
			b = pixel->b >> 1;
		}
		break;
		case 3: {
			rgb_t *pixel = line + count;
			r = pixel->r >> 4;
			g = pixel->g >> 4;
			b = pixel->b >> 4;
		}
		break;

		case 4: {
			rgba_t *pixel = line + count;
			if (pixel->a)
				continue;
			r = pixel->r >> 4;
			g = pixel->g >> 4;
			b = pixel->b >> 4;
		}
		break;
		}
		count += bpp;

		if (r == 0xa && g == 0xa && b == 0xa)
			continue;

		set_pixel444(x + offset_x, y, r, g, b);

	}
	y++;
}


void framebuffer444_draw_image(image_t *image, int x_offset, int y_offset) {
	uint8_t *line = image->pixel_data;
	for (int y = 0; y < image->height; y++) {
		framebuffer444_drawline(x_offset, y + y_offset, image->width, line,
				image->bytes_per_pixel);
		line += (image->width * image->bytes_per_pixel);
	}
}
*/

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
		for (int y = 0; y < 64; y++) {
			for (int x = 0; x < 132; x++)
				set_pixel444(x, y, r, 0x0, 0x0);
		}
		framebuffer_apply();
	}
	for (int r = 15; r; r--) {
		for (int y = 0; y < 64; y++) {
			for (int x = 0; x < 132; x++)
				set_pixel444(x, y, r, 0x0, 0x0);
		}
		framebuffer_apply();
	}
}



//---
//static_assert(sizeof(line444_t) == 198, "CHECK LINE SIZE");
#endif


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


#include "graphics.h"

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
