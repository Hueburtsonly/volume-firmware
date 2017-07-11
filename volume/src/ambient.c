/*
 * ambient.c
 *
 *  Created on: 4 Jun 2017
 *      Author: Hueburtsonly
 */

#include "chip.h"
#include "cdc.h"

#define PIN_ADC 22
#define ADC_ADC 6

#define N 380

void ambient_init() {
	LPC_IOCON->PIO0[PIN_ADC] = 0x01; // ADC, 0x80 for GPIO
}

uint16_t ambient_result = 0;

void ambient_measure() {
	static uint32_t partial = 0;
	static uint16_t count = 0;


		LPC_ADC->CR = (1 << 24) | (101 << 8) | (1 << ADC_ADC);

		while (!(LPC_ADC->GDR & (1 << 31)));

		partial += (LPC_ADC->GDR >> 6) & ((1 << 10) - 1);

	if (++count == N) {
		count = 0;
		ambient_result = partial/10;
		partial = 0;
	}
}
