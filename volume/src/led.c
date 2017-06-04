/*
 * led.c
 *
 *  Created on: 27 Dec 2016
 *      Author: Hueburtsonly
 */

#include "chip.h"
#include "led.h"

#define ALL_LEDS ((1<<LED_RED) | (1<<LED_GREEN) | (1<<LED_BLUE))

void led_init() {
	//LPC_IOCON->PIO0[LED_RED]; invert
	//LPC_IOCON->PIO0[LED_GREEN]; invert
	//LPC_IOCON->PIO0[LED_BLUE]; invert

	LPC_GPIO->DIR[0] |= ALL_LEDS;
	LPC_GPIO->SET[0] = ALL_LEDS;
}

void led_on(uint8_t led) {
	LPC_GPIO->CLR[0] = ALL_LEDS & (1 << led);
}

void led_off(uint8_t led) {
	LPC_GPIO->SET[0] = ALL_LEDS & (1 << led);
}
