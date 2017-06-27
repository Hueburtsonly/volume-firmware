/*
 * tlc5928.c
 *
 *  Created on: 28 May 2017
 *      Author: Hueburtsonly
 */

#include "chip.h"
#include "led.h"


#define LAT 11
#define SCL 12
#define SI0 13
#define SI1 14
#define BLANK 16

volatile uint32_t datums[8];


void tlc5928_init() {

    LPC_IOCON->PIO0[LAT] = 0x81;
    LPC_IOCON->PIO0[SCL] = 0x81;
    LPC_IOCON->PIO0[SI0] = 0x81;
    LPC_IOCON->PIO0[SI1] = 0x81;

	LPC_GPIO->DIR[0] |= (1 << LAT) | (1 << SCL) | (1 << SI0) | (1 << SI1) | (1 << BLANK);
	LPC_GPIO->SET[0] = (1 << LAT) | (1 << SCL) | (1 << SI0) | (1 << SI1) | (0 << BLANK);
}

void tlc5928_broadcast(uint16_t state) {
	for (int board = 7; board >= 0; --board) {
		datums[board] = (uint32_t)state | ((uint32_t)state << 16);
	}
	tlc5928_send();
}

void tlc5928_send() {
	// 3.17676 kHz with 16 ch, 16 pins single file
	// 4.32870 kHz with 16 ch, 16 pins double file from uint16_t pair
	// 5.61152 kHz with 16 ch, 16 pins double file from uint32_t
	int board;
	int pin;
	LPC_GPIO->CLR[0] = (1 << LAT);
	__NOP(); __NOP(); __NOP();
	for (board = 7; board >= 0; --board) {
		uint32_t v = datums[board];
		//LPC_GPIO->CLR[0] = (1 << );
		for (pin = 0; pin < 16; pin++) {
			__NOP(); __NOP(); __NOP();
			LPC_GPIO->CLR[0] = (1 << SCL);
			__NOP(); __NOP(); __NOP();
			LPC_GPIO->W[0][SI0] = v & 0x8000;
			LPC_GPIO->W[0][SI1] = v & 0x80000000;
			v <<= 1;
			LPC_GPIO->SET[0] = (1 << SCL);
			__NOP(); __NOP(); __NOP();
		}
		__NOP(); __NOP(); __NOP();
		LPC_GPIO->CLR[0] = (1 << SCL);
		__NOP(); __NOP(); __NOP();
	}
	__NOP(); __NOP(); __NOP();
	LPC_GPIO->SET[0] = (1 << LAT);
	__NOP(); __NOP(); __NOP();

	LPC_GPIO->CLR[0] = (1 << LAT);
}
