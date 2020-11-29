/*
 * graphics.h
 *
 *  Created on: 17 apr. 2020
 *      Author: andre
 */

#ifndef GRAPHICS_GRAPHICS_H_
#define GRAPHICS_GRAPHICS_H_

#include <stdint.h>

#pragma pack(push,1)

typedef struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
  unsigned char	 pixel_data[];
} image_t;

typedef struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
  unsigned char	 rle_pixel_data[];
} rle_image_t;

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} rgba_t;

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} rgb_t;

typedef struct {
	unsigned int b :5;
	unsigned int g :6;
	unsigned int r :5;
} rgb565_t;


typedef struct {
	uint8_t count;
	uint8_t value;
} rle_t;

typedef struct {
	uint8_t count;
	rgb_t pixel;
} rle_rgb_t;


typedef struct {
	uint8_t count;
	rgba_t pixel;
} rle_rgba_t;


typedef struct {
	uint8_t count;
	rgb565_t pixel;
} rle_rgb565_t;


#pragma pack(pop)

extern const image_t* sheep;
extern const image_t* itph;


#endif /* GRAPHICS_GRAPHICS_H_ */
