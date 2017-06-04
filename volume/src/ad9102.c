/*
 * ad9102.c
 *
 *  Created on: 23 Dec 2016
 *      Author: Hueburtsonly
 */

// !Trigger = PIO0_17
// !Reset = PIO0_16
// !CS = PIO0_23

#define PIN_TRIGGER 17
#define PIN_RESET 16
#define PIN_CS 23

#include "chip.h"
#include "stdint.h"
#include "spi.h"


void ad9102_reset() {
	// IOCON is fine by default for PIO0_16/17, and CS is handled by spi.c
	LPC_GPIO->DIR[0] |= (1 << PIN_TRIGGER) | (1 << PIN_RESET);

	// !Reset low
	LPC_GPIO->CLR[0] = (1 << PIN_RESET);

	// !Trigger high
	LPC_GPIO->SET[0] = (1 << PIN_TRIGGER);
}

static inline void send(uint16_t addr, uint16_t value) {
	uint8_t buf[4];
	buf[0] = 0x7f & (uint8_t)(addr >> 8);
	buf[1] = (uint8_t)(addr);
	buf[2] = (uint8_t)(value >> 8);
	buf[3] = (uint8_t)(value);

	spi_write(PIN_CS, buf, 4);
}

static inline uint16_t recv(uint16_t addr) {
	uint8_t buf[2];
	buf[0] = 0x80 | (0x7f & (uint8_t)(addr >> 8));
	buf[1] = (uint8_t)(addr);

	uint8_t ret[2];
	ret[0] = 0x11;
	ret[1] = 0x11;

	spi_write_read(PIN_CS, buf, 2, ret, 2);

	return (((uint16_t)ret[0]) << 8) | ((uint16_t)ret[1]);
}

static const uint32_t RAM_LEN = 1800;
static const uint32_t RAM_REPEAT = 500;


void ad9102_init() {
	LPC_TIMER32_0->PR = 48 - 1;
	LPC_TIMER32_0->TC = -1;



	// !Reset high
	LPC_GPIO->SET[0] = (1 << PIN_RESET);

	volatile int ijk;
		for (ijk = 0; ijk < 10000; ijk++);

	// Enable DOUT
	send(0x00, 0x4422);

	// Optional: adjust REFIO
	//send(0x03, );

	// Enable writes to RAM
	send(0x1E, 0x0004);

	// Optional: limit pattern loops
	send(0x1F, 0x0000);  // 0x1 here for limited number of cycles

	// Optional: PATTERN_DELAY
	send(0x20, 0x000F - 1 + 2);

	// Optional: DAC_DIG_OFFSET
	//send(0x25, 0x0000);

	// DDS (sine) waveform modulated by RAM
	send(0x27, 0x0133); // TODO: 0x0133

	// PATTERN_PERIOD LSB = 16 DAC clocks
	// START_DELAY LSB = 16 DAC clocks
	send(0x28, 0x0111); // TODO: 0x01ff

	// PATTERN_PERIOD = 0xffff
	send(0x29, 0xffff);

	// DAC_REPEAT_CYCLE = 0xff
	send(0x2B, 2);

	// Optional DOUT_START delay
	send(0x2C, 0xF);

	// DOUT defined by pattern gen
	send(0x2D, 0x0010); // TODO: 0x0010

	// Optional DAC constant
	send(0x31, 0x0);

	// DAC digital gain = 1.0????? QUESTIONABLE
	send(0x35, 0x4000);

	// Sawtooth config is 0x37
	//send(0x37, );

	// DDS tuning word for 13.37 MHz
	uint32_t DDSTW = 1246174;
	DDSTW =          1118481/6  * 5;
	send(0x3E, (uint16_t)(DDSTW >> 8));
	send(0x3F, (uint16_t)(DDSTW << 8));


	const uint32_t RAM_TLEN = RAM_LEN * RAM_REPEAT;
	const uint32_t DDSPHASE = (~((RAM_TLEN - 1) * DDSTW)) >> 8;


	// DDS phase
	send(0x43, DDSPHASE);

	// Delay is only at start of first pattern
	send(0x44, 0x0002);

	// SRAM address counter = MSB of DDS
	send(0x45, 0x0000); // TODO: 0x0004

	// RAM address 0x6000 - 0x6fff
	send(0x5d, 0x000 << 4);
	send(0x5e, (RAM_LEN-1) /*0xfff*/ << 4);
	send(0x5f, 4096);

	// 6         *                                     //
	// 5        * *                                     //
	// 4       *   * *                                   //
	// 3    * *     * *                                   //
	// 2   * *   |     *                                   //
	// 1  *             *                                //
	// 0 *               *                              //
	//   0123456789abcdef
	//   +++-++++---+----
	// Clock = 13.37 MHz
	// 4096 samples
	// Overall period = 306.357 us = 3.264160156 kHz

	const char* prog = "+++-++++---+----";

	// 14-bit signed samples
	// 8091 to -8091
	// 8091 / 6 / 0xff
	// step 5 each sample
	uint16_t current = 0;
	uint16_t i = 0;
	for (; i <= 0xfff; i++) {
		current += (prog[i >> 8] == '+') ? 5 : -5;
		//send(0x6000 | i, (i < 0x800) ? current : -current);
		//send(0x6000 | i, 4 * ((i < 0x800) ? current : -current));
		//send(0x6000 | i, ((i < 6) && (i & 0x1)) ? -0x7fff : 0x7fff);
		send(0x6000 | i, ((i < 0) && (i & 0x1)) ? -0x7fff : 0x7fff);
	}

	// Enable run
	send(0x1E, 0x0001);

	// Strobe update
	send(0x1D, 0x0001);


	/*for (;;) {
		ad9102_sine_burst(26000000);

	}*/
/*
	DDSTW = 2796203;

	for (;;) {
		send(0x3E, (uint16_t)(DDSTW >> 8));
		send(0x3F, (uint16_t)(DDSTW << 8));
		send(0x1D, 0x0001);
		DDSTW = DDSTW + 1 + (DDSTW >> 20);
	}*/
}


void ad9102_sine_burst(int32_t freq, int32_t phase) {

	const uint32_t RAM_TLEN = RAM_LEN * RAM_REPEAT;

	uint32_t DDSTW = /*1246174*/ (((uint64_t)freq) << 24) / (uint64_t)180000000;
	send(0x3E, (uint16_t)(DDSTW >> 8));
	send(0x3F, (uint16_t)(DDSTW << 8));


	//const uint32_t RAM_LEN = 10;
	//const uint32_t RAM_REPEAT = 2;
	//const uint32_t RAM_TLEN = RAM_LEN * RAM_REPEAT;
	const uint32_t DDSPHASEx = (~((RAM_TLEN - 1) * DDSTW)) >> 8;


	// DDS phase
	send(0x43, DDSPHASEx /*+ 0x4000*/ + phase);


	send(0x1D, 0x0001);

	while (LPC_TIMER32_0->TC < 10000);
	LPC_TIMER32_0->TCR = 0; // stop
	LPC_TIMER32_0->TC = 0;
	LPC_TIMER32_0->PC = 29;


	// !Trigger low
	LPC_GPIO->CLR[0] = (1 << PIN_TRIGGER);

	LPC_TIMER32_0->TCR = 1; // start
	while (LPC_TIMER32_0->TC < 4995);

	LPC_GPIO->SET[0] = (1 << PIN_TRIGGER);




}


void ad9102_dout(uint8_t on) {

	send(0x2D, on ? 0x0020 : 0); // TODO: 0x0010
	send(0x1D, 0x0001);


}

void ad9102_const(int32_t dc) {
	send(0x31, dc << 4);
	send(0x1D, 0x0001);



	LPC_GPIO->SET[0] = (1 << PIN_TRIGGER);

	volatile int ijk;
		for (ijk = 0; ijk < 10000; ijk++);

	LPC_GPIO->SET[0] = (1 << PIN_TRIGGER);
}

uint16_t ad9102_pull_test() {
	return recv(0x60);
}
