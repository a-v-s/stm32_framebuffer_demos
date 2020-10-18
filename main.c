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

/*
 * WS2812 DMA library demo for STM32F1 (STM32CubeF1 library)
 */


#define PERIOD (0x3F)

#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_rcc.h"

#include <stdbool.h>

volatile bool updateLEDs = false;
volatile int updateVALs[2];


/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 72000000
 *            HCLK(Hz)                       = 72000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 2
 *            APB2 Prescaler                 = 1
 *            HSE Frequency(Hz)              = 8000000
 *            HSE PREDIV1                    = 1
 *            PLLMUL                         = 9
 *            Flash Latency(WS)              = 2
 * @param  None
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_ClkInitTypeDef clkinitstruct = { 0 };
	RCC_OscInitTypeDef oscinitstruct = { 0 };
	RCC_PeriphCLKInitTypeDef rccperiphclkinit = { 0 };

	/* Enable HSE Oscillator and activate PLL with HSE as source */
	oscinitstruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	oscinitstruct.HSEState = RCC_HSE_ON;
	oscinitstruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	oscinitstruct.PLL.PLLMUL = RCC_PLL_MUL9;

	oscinitstruct.PLL.PLLState = RCC_PLL_ON;
	oscinitstruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

	if (HAL_RCC_OscConfig(&oscinitstruct) != HAL_OK) {
		/* Start Conversation Error */
		//Error_Handler();
	}

	/* USB clock selection */
	rccperiphclkinit.PeriphClockSelection = RCC_PERIPHCLK_USB;
	rccperiphclkinit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
	HAL_RCCEx_PeriphCLKConfig(&rccperiphclkinit);

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	 clocks dividers */
	clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;
	clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2) != HAL_OK) {
		/* Start Conversation Error */
		//Error_Handler();
	}
}

void delay_ms(uint32_t ms) {
	HAL_Delay(ms);
}

void SysTick_Handler() {
	HAL_IncTick();
}

static TIM_HandleTypeDef hTim4 = { 0 };
static TIM_HandleTypeDef hTim1 = { 0 };
int qdec_init() {

	// Enable Timer 1 Clock
	__HAL_RCC_TIM1_CLK_ENABLE();

	// Enable Timer 4 Clock
	__HAL_RCC_TIM4_CLK_ENABLE();

	// Enable GPIO Port A Clock
	__HAL_RCC_GPIOA_CLK_ENABLE();

	// Enable GPIO Port B Clock
	__HAL_RCC_GPIOB_CLK_ENABLE();

	TIM_Encoder_InitTypeDef encoderConfig = { 0 };
	encoderConfig.EncoderMode = TIM_ENCODERMODE_TI2;

	encoderConfig.IC1Filter = 0x08;//0x00;
	encoderConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
	encoderConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	encoderConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;

	encoderConfig.IC2Filter = encoderConfig.IC1Filter;
	encoderConfig.IC2Polarity = encoderConfig.IC1Polarity;
	encoderConfig.IC2Prescaler = encoderConfig.IC1Prescaler;
	encoderConfig.IC2Selection = encoderConfig.IC1Selection;

	hTim1.Instance = TIM1;
	hTim1.Init.Period = 0xFF;

	HAL_TIM_Encoder_Init(&hTim1, &encoderConfig);

	GPIO_InitTypeDef GPIO_InitStruct;

	// Common configuration for all channels
	GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//HAL_TIM_Encoder_Start_IT(&hTim1, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start_IT(&hTim1, TIM_CHANNEL_1);


	NVIC_EnableIRQ(TIM1_CC_IRQn);

	hTim4.Instance = TIM4;
	hTim1.Init.Period = 0xFF;
	HAL_TIM_Encoder_Init(&hTim4, &encoderConfig);

	GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	//HAL_TIM_Encoder_Start_IT(&hTim4, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start_IT(&hTim4, TIM_CHANNEL_1);

	NVIC_EnableIRQ(TIM4_IRQn);

}

int btn_init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;
	// Common configuration for all channels
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	GPIO_InitStruct.Pin = GPIO_PIN_10;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_8;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void TIM1_CC_IRQHandler() {
	HAL_TIM_IRQHandler(&hTim1);
}

void TIM4_IRQHandler() {
	HAL_TIM_IRQHandler(&hTim4);
}

void EXTI15_10_IRQHandler(void){

	bool btnBrightness = __HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_8);
	bool btnColour =  __HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_10);
	__HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_All);

	if (btnColour) {
		for (int i = 1 ; i <= 4; i++)
			if (updateVALs[0] < (i*PERIOD/4)) {
				updateVALs[0] = (i*PERIOD/4);
				updateLEDs = true;
				return;
			}
		if (updateVALs[0] == PERIOD) {
			updateVALs[0] = 0;
			updateLEDs = true;
		}

	}

	if (btnBrightness) {
		for (int i = 1 ; i <= 4; i++)
			if (updateVALs[1] < (i*PERIOD/4)) {
				updateVALs[1] = (i*PERIOD/4);
				updateLEDs = true;
				return;
			}
		if (updateVALs[1] == PERIOD) {
			updateVALs[1] = 0;
			updateLEDs = true;
		}
	}

}

void convertColour(int colour,uint8_t * wwa){
	uint8_t *a = wwa+1;
	uint8_t *w = wwa + 2;
	uint8_t *c = wwa + 0;
	float m = (float) (colour & 0x7F) / (float) 0x7F;
	if (colour & 0x80) {
		*a = 0;
		*c = m * (float) 0xFF;
		*w = 0xFF - *c;
	} else {
		*c = 0;
		*w = m * (float) 0xFF;
		*a = 0xFF - *w;
	}
}

void applyLEDS(int colour, int brightness) {
	uint8_t wwa[3];
	convertColour(colour,wwa);

	wwa[0] = (float) brightness / (float) 0xFF * (float) wwa[0];
	wwa[1] = (float) brightness / (float) 0xFF * (float) wwa[1];
	wwa[2] = (float) brightness / (float) 0xFF * (float) wwa[2];

	// using 35 leds
	for (int i = 0; i < 35; i++)
		ws2812_fill_buffer_decompress(3*i, 3, wwa);
	//while (ws2812_is_busy());
	//ws2812_apply(3*35);
	ws2812_apply_channel(3*35, 0);
	while (ws2812_is_busy());
	ws2812_apply_channel(3*35, 1);
	while (ws2812_is_busy());
}


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	int8_t val = htim->Instance->CNT;
	if (val)
		htim->Instance->CNT = 0;
	else
		return;
	__DSB();

	int *pval = NULL;
	switch ((int) htim->Instance) {
	case (int) TIM1:
		pval = updateVALs;
		break;
	case (int) TIM4:
		pval = updateVALs + 1;
		break;
	default:
		return;
	}
	int newval = *pval + val;

	if (newval > PERIOD) newval = PERIOD;
	if (newval < 0 ) newval = 0;
	*pval = newval;

	updateLEDs = true;
	//applyLEDS(updateVALs[0]<<2, updateVALs[1]<<2);
}

int main() {
	HAL_Init();
	SystemClock_Config();
	SystemCoreClockUpdate();

	ws2812_init();
	while (ws2812_is_busy());

	applyLEDS(0x7F,0xFF);

	qdec_init();
	//btn_init();

	int val;
	while (1) {
		if (updateLEDs) {
			updateLEDs = false;
			applyLEDS(updateVALs[0]<<2, updateVALs[1]<<2);
		}
	};;
}
