/*
 * touch.c
 *
 *  Created on: 11 May 2017
 *      Author: Hueburtsonly
 */

#include "touch.h"
#include "chip.h"
#include <stdint.h>
#include "led.h"
#include "cdc.h"

#define PIN_ADC 23

#define ADC_ADC 7 // TODO

#define N 380
static uint16_t recent[N];
static uint8_t index;
static uint16_t decimate;
static uint16_t threshold = 0;


void touch_init() {

	LPC_SYSCTL->PDRUNCFG &= ~(1 << 4);

	LPC_IOCON->PIO0[PIN_ADC] = 0x80; // GPIO, 0x01 for ADC

	LPC_GPIO->DIR[0] |= (1 << PIN_ADC);
	LPC_GPIO->SET[0] = (1 << PIN_ADC);


	// clkdiv = 10, sel = 6, software controlled mode, 10 bit
	LPC_ADC->CR = (10 << 8) | (1 << ADC_ADC);

	int i;
	threshold = 0xffff;
	for (i = 0; i < N; i++) {
		recent[i] = threshold;
	}

}


uint16_t touch_result = 0;

void touch_measure() {
	static uint32_t partial = 0;
	static uint16_t count = 0;


	__disable_irq();
	LPC_IOCON->PIO0[PIN_ADC] = 0x01; // ADC, 0x80 for GPIO


	// clkdiv = 10, sel = 6, software controlled mode, 10 bit, start now
	LPC_ADC->CR = (1 << 24) | (101 << 8) | (1 << ADC_ADC);

	__enable_irq();

	while (!(LPC_ADC->GDR & (1 << 31)));

	//while ((LPC_ADC->STAT & (1 << ADC_ADC)) == 0);

	LPC_IOCON->PIO0[PIN_ADC] = 0x80; // GPIO, 0x01 for ADC

	partial += (LPC_ADC->GDR >> 6) & ((1 << 10) - 1);

	if (++count == N) {
		count = 0;
		touch_result = partial/10;
		partial = 0;
	}
}

/*
uint16_t touch_sample_old() {

	uint16_t result = 0;



	uint16_t i;
	for (i = 0; i < 48; i++) {

		__disable_irq();
		LPC_IOCON->PIO0[PIN_ADC] = 0x01; // ADC, 0x80 for GPIO


		// clkdiv = 10, sel = 6, software controlled mode, 10 bit, start now
		LPC_ADC->CR = (1 << 24) | (101 << 8) | (1 << ADC_ADC);

		__enable_irq();

		while (!(LPC_ADC->GDR & (1 << 31)));

		//while ((LPC_ADC->STAT & (1 << ADC_ADC)) == 0);

		LPC_IOCON->PIO0[PIN_ADC] = 0x80; // GPIO, 0x01 for ADC

		result += (LPC_ADC->GDR >> 6) & ((1 << 10) - 1);

		uint16_t ijk;
		for (ijk = 0; ijk < 1; ijk++);


	}

	if ((++decimate & 0x7f) == 0) {
		recent[index = (index+1) % N] = result;
		threshold = 0xffff;
		for (i = 0; i < N; i++) {
			if (threshold > recent[i]) {
				threshold = recent[i];
			}
		}
		threshold = threshold + (threshold >> 3);
	}

	if (result > threshold) {
		led_off(LED_RED);
		//cdc_write_str("++ ");
	} else {
		led_on(LED_RED);
		//cdc_write_str("   ");
	}

	//cdc_write_int(result);
	//cdc_write_str("   ");
	//cdc_write_int(threshold);
	//cdc_write_str("\r\n");

	return result;
}
*/
