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

void ambient_init() {
	LPC_IOCON->PIO0[PIN_ADC] = 0x01; // ADC, 0x80 for GPIO
}

void ambient_measure() {
	// TODO

	for (;;) {
		uint32_t result = 0;
		uint16_t i;
		for (i = 0; i < 1000; i++) {

			LPC_ADC->CR = (1 << 24) | (101 << 8) | (1 << ADC_ADC);

			while (!(LPC_ADC->GDR & (1 << 31)));

			result += (LPC_ADC->GDR >> 6) & ((1 << 10) - 1);
		}

		cdc_write_uint(result);
		cdc_write_str("\r\n");
	}
}
