/*
 * spi.c
 *
 *  Created on: 27 Dec 2016
 *      Author: Hueburtsonly
 */

#include "chip.h"

#define CS_MASK ((1 << 13) | (1 << 23) | (1 << 2))

// Port 1!!!
#define SCLK 15
// Port 0.
#define SDIO 21

void spi_init() {
	// sclk output low
	// sdio output ??
	// !pllcs output high
	// !wgcs output high

	LPC_IOCON->PIO0[13] = 0x1; // GPIO
	LPC_IOCON->PIO0[21] = 0x88; // GPIO
	LPC_GPIO->DIR[1] |= 1 << SCLK;
	LPC_GPIO->CLR[1] = 1 << SCLK;
	LPC_GPIO->DIR[0] |= CS_MASK | (1 << SDIO);
	LPC_GPIO->SET[0] = CS_MASK;
}

void spi_write(uint8_t cs, const uint8_t* data, uint8_t len) {
	uint32_t cs_pin = (1 << cs) & CS_MASK;
	while (cs_pin == 0);

	LPC_GPIO->CLR[0] = cs_pin;

	uint8_t i = 0;
	for (; i < len; i++) {
		uint8_t byte = data[i];

		uint8_t j = 0;
			for (; j < 8; j++) {
			if (byte & 0x80) {
				LPC_GPIO->SET[0] = 1 << SDIO;
			} else {
				LPC_GPIO->CLR[0] = 1 << SDIO;
			}
			LPC_GPIO->SET[1] = 1 << SCLK; // Trigger read

			byte <<= 1;

			LPC_GPIO->CLR[1] = 1 << SCLK;
		}
	}

	LPC_GPIO->SET[0] = cs_pin;
}


void spi_write_read(uint8_t cs, const uint8_t* wdata, uint8_t wlen, uint8_t* rdata, uint8_t rlen) {
	uint32_t cs_pin = (1 << cs) & CS_MASK;
	while (cs_pin == 0);

	LPC_GPIO->CLR[0] = cs_pin;

	uint8_t i = 0;
	for (; i < wlen; i++) {
		uint8_t byte = wdata[i];

		uint8_t j = 0;
		for (; j < 8; j++) {
			if (byte & 0x80) {
				LPC_GPIO->SET[0] = 1 << SDIO;
			} else {
				LPC_GPIO->CLR[0] = 1 << SDIO;
			}
			LPC_GPIO->SET[1] = 1 << SCLK; // Trigger read

			byte <<= 1;

			LPC_GPIO->CLR[1] = 1 << SCLK;
		}
	}

	LPC_GPIO->DIR[0] &= ~(1 << SDIO);

	i = 0;
	for (; i < rlen; i++) {
		uint8_t byte = 0;

		uint8_t j = 0;
		for (; j < 8; j++) {
			byte <<= 1;
			LPC_GPIO->SET[1] = 1 << SCLK; // Trigger read

			if (LPC_GPIO->PIN[0] & (1 << SDIO)) byte |= 1;

			LPC_GPIO->CLR[1] = 1 << SCLK;
		}

		rdata[i] = byte;
	}

	LPC_GPIO->SET[0] = cs_pin;
	LPC_GPIO->DIR[0] |= (1 << SDIO);
}
