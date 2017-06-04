/*
 * lmh6518.c
 *
 *  Created on: 5 Jan 2017
 *      Author: Hueburtsonly
 */

#include "chip.h"
#include "stdint.h"
#include "spi.h"

// Reset = PIO0_14 YOLO
// !CS = PIO0_11 YOLO
// PLL LOCK = PIO0_12
// YOLO:
#define PIN_CS_1 11
#define PIN_CS_2 11
#define PIN_LOCKED 12

#define BANDWIDTH_FULL 0b000
#define BANDWIDTH_20   0b001
#define BANDWIDTH_100  0b010
#define BANDWIDTH_200  0b011
#define BANDWIDTH_350  0b100
#define BANDWIDTH_650  0b101
#define BANDWIDTH_750  0b110

void lmh6518_reset() {
	// YOLO all this
	// IOCON is fine by default for PIO0_13/14, and CS is handled by spi.c
	//LPC_GPIO->DIR[0] |= (1 << PIN_RESET);

	// reset high
	//LPC_GPIO->SET[0] = (1 << PIN_RESET);

	// locked pull-down
	//LPC_IOCON->PIO0[PIN_LOCKED] = 0x28; // Pull-down, with hysteresis
}

static inline void send(uint8_t cs, uint16_t value) {
	uint8_t buf[3];
	buf[0] = 0;
	buf[1] = (uint8_t)(value >> 8);
	buf[2] = (uint8_t)(value);

	spi_write(cs, buf, 3);
}

// Allowed gain values are 0, 2, ..., 40. Corresponding to -1.2dB, 0.8dB, ... , 38.8dB.
// Allowed bandwidth values are one of the BANDWIDTH_* constants.
void lmh6518_set(int8_t cs, int8_t gain, int8_t bandwidth) {
	while (gain < 0);
	while (gain > 40);
	while (gain & 1);
	while (bandwidth < 0);
	while (bandwidth > 6);

	uint16_t command = (1 << 10) | (bandwidth << 6);
	if (gain >= 20) {
		gain -= 20;
		command |= 1 << 4;
	}
	// Gain = 20 -> -0 dB atten (0b0000)
	// Gain = 0 -> -20 dB atten (0b1010)
	command |= (((20 - gain) / 2) & 0xf) << 0;

	send(cs, command);
}

void lmh6518_init() {
  lmh6518_set(PIN_CS_1, 0, BANDWIDTH_20);
  lmh6518_set(PIN_CS_2, 0, BANDWIDTH_20);
}

// Allowed gain values are 0, 2, ..., 80. Corresponding to -2.4dB, -0.4dB, ... 77.6dB.
// Allowed bandwidth values are one of the BANDWIDTH_* constants.
void lmh6518_set_both(int8_t gain, int8_t bandwidth1, int8_t bandwidth2) {
	while (gain < 0);
	while (gain > 80);
	while (gain & 1);

	// gain  gain1 gain2
	//   0     0     0
	//   2     0     2
	//   4     2     2
	//   6     2     4
	//   8     4     4
	//  44    40     4
	//  80    40    40

	int8_t gain1 = 0;
	int8_t gain2 = 0;
	if (gain >= 44) {
		gain1 = 40;
		gain2 = gain - gain1;
	} else if (gain >= 8) {
		gain2 = 4;
		gain1 = gain - gain2;
	} else {
		gain1 = (gain >> 2) << 1;
		gain2 = gain - gain1;
	}
  lmh6518_set(PIN_CS_1, gain1, bandwidth1);
  lmh6518_set(PIN_CS_2, gain2, bandwidth2);
}
