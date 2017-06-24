/*
 * ambient.c
 *
 *  Created on: 4 Jun 2017
 *      Author: Hueburtsonly
 */

#include "chip.h"

#define PIN_ADC 22
#define ADC_ADC 6

void ambient_init() {
	LPC_IOCON->PIO0[PIN_ADC] = 0x01; // ADC, 0x80 for GPIO
}

void ambient_measure() {
	// TODO

	LPC_ADC->CR = (1 << 24) | (101 << 8) | (1 << ADC_ADC);

	while (!(LPC_ADC->GDR & (1 << 31)));

	LPC_ADC->GDR;
}
