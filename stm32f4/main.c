/*

 File: 		main.c
 Author:	André van Schoubroeck
 License:	MIT


 MIT License

 Copyright (c) 2017, 2018, 2019, 2020 André van Schoubroeck <andre@blaatschaap.be>

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 */

#define PERIOD (0x3F)

#include "stm32f4xx.h"

#include <stdbool.h>

#include "graphics.h"

void Error_Handler() {
	__BKPT(0);
}



void SystemClock_Config() {
	//SystemClock_HSE25_OUT84_Config();
	//SystemClock_HSI_OUT84_Config();

	SystemClock_HSE25_OUT96_Config();
}

void SysTick_Handler() {
	HAL_IncTick();
}

int main() {
#ifdef SEMI
	initialise_monitor_handles();
	printf("Hello world!\n");
#endif
	HAL_Init();
	SystemClock_Config();
	SystemCoreClockUpdate();

	display_init();
	set_565();

	int framecount = 0;
	int fps = 0;
	int lastTick = HAL_GetTick();
	while (1) {

		for (int y = 0; y < 32; y++) {

			draw_background();
			draw_test();
			//draw_plain_background();
			draw_image(sheep, 0, y);
			//draw_image(itph, 0, y);
			print_fps(fps);

			// Plane background + sheep
			// -O0 13 fps @ 84 MHz
			// -O2 28 fps @ 84 MHz / 32 fps @ 100 MHz
			// -Os 24 fps @ 84 MHz


			framebuffer_apply();
			framecount++;
			if ((HAL_GetTick() - lastTick) > 1000) {
				fps = framecount;
				framecount = 0;
				lastTick = HAL_GetTick();
			}
		}

		for (int y = 31; y; y--) {
			draw_background();
			draw_test();
			//draw_plain_background();
			draw_image(sheep, 0, y);
			//draw_image(itph, 0, y);
			print_fps(fps);
			framebuffer_apply();
			framecount++;
			if ((HAL_GetTick() - lastTick) > 1000) {
				fps = framecount;
				framecount = 0;
				lastTick = HAL_GetTick();
			}
		}

	}

}
