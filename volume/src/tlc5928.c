/*
 * tlc5928.c
 *
 *  Created on: 28 May 2017
 *      Author: Hueburtsonly
 */

#include "chip.h"
#include "led.h"

#define SI LED_GREEN
#define SCL LED_BLUE
#define LAT LED_RED

volatile uint32_t datums[8];


void init5928() {
	int ch;
	for (ch = 7; ch >= 0; --ch) {
		datums[ch] = ch | 0xf000;
	}
}

void test5928() {
	// 3.17676 kHz with 16 ch, 16 pins single file
	// 4.32870 kHz with 16 ch, 16 pins double file from uint16_t pair
	// 5.61152 kHz with 16 ch, 16 pins double file from uint32_t
	int ch;
	int pin;
	LPC_GPIO->CLR[0] = (1 << LAT);
	for (ch = 7; ch >= 0; --ch) {
		uint32_t v = datums[ch];
		//LPC_GPIO->CLR[0] = (1 << );
		for (pin = 0; pin < 15; pin++) {
			LPC_GPIO->CLR[0] = (1 << SCL);
			LPC_GPIO->W[0][SI] = v & 0x8000;
			LPC_GPIO->W[0][SI] = v & 0x80000000;
			v <<= 1;
			LPC_GPIO->SET[0] = (1 << SCL);
		}
	}
	LPC_GPIO->SET[0] = (1 << LAT);
}
