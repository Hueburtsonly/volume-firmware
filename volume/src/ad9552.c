/*
 * ad9552.c
 *
 *  Created on: 23 Dec 2016
 *      Author: Hueburtsonly
 */

#include "chip.h"
#include "led.h"
#include "stdint.h"
#include "spi.h"
#include "ad9552.h"

// Reset = PIO0_14
// !CS = PIO0_13
// PLL LOCK = PIO0_12

#define PIN_LOCKED 12

//#define ALT_CHIP

#ifndef ALT_CHIP
// Onboard chip
#define PIN_RESET 14
#define PIN_CS 13
#define PIN_ALT_RESET 20
#define PIN_ALT_CS 2
#else
// Addon chip
#define PIN_RESET 20
#define PIN_CS 2
#define PIN_ALT_RESET 14
#define PIN_ALT_CS 13
#endif

void ad9552_reset() {

	LPC_IOCON->PIO0[14] = 0x71;

	// IOCON is fine by default for PIO0_13/14, and CS is handled by spi.c
	LPC_GPIO->DIR[0] |= (1 << PIN_RESET) | (1 << PIN_ALT_RESET);

	// reset high
	LPC_GPIO->SET[0] = (1 << PIN_RESET) | (1 << PIN_ALT_RESET);

	// locked pull-down
	LPC_IOCON->PIO0[PIN_LOCKED /*12*/] = 0xa9; // GPIO, pull-down, hystersis, digital

	volatile int i;
		for (i = 0; i < 100000; i++);
}

static inline void send(uint8_t addr, uint8_t value) {
/*

1. Pins: The SSP/SPI pins must be configured in the IOCON register block.
2. Power: In the SYSAHBCLKCTRL register, set bit 11 for SSP0 and bit 18 for SSP1
(Table 24).
3. Peripheral clock: Enable the SSP0/SSP1 peripheral clocks by writing to the
SSP0/1CLKDIV registers (Table 25/Table 27).
4. Reset: Before accessing the SSP/SPI block, ensure that the SSP0/1_RST_N bits (bit
0 and bit 2) in the PRESETCTRL register (Table 8) are set to 1. This de-asserts the
reset signal to the SSP/SPI block.

 */

	uint8_t buf[3];
	buf[0] = 0;
	buf[1] = addr;
	buf[2] = value;

	spi_write(PIN_CS, buf, 3);
}

#ifndef ALT_CHIP
// Onboard chip
void ad9552_init() {
	// reset low
	LPC_GPIO->CLR[0] = (1 << PIN_RESET);

	// N = 189
	send(0x11, 189);

	// Disable fractional-N
	send(0x14, 0x0c);

	//// P0 = 7, P1 = 3, en spi ctrl
	send(0x17, 0x00);
	send(0x18, 0x1b);
	send(0x19, 0xa0);

	// P0 = 7, P1 = 3*15, en spi ctrl
	//send(0x17, 0x01);
	//send(0x18, (45-32) << 3 | 0b011);
	//send(0x19, 0xa0);


	// OUT1 = LVDS
	send(0x32, 0xa1);

	// OUT2 = Complementary CMOS
	send(0x34, 0x81);

	// Strobe update
	ad9552_vco_band_manual(41);
}
#else
// Alt chip
void ad9552_init() {
	// reset low
	LPC_GPIO->CLR[0] = (1 << PIN_RESET);

	// N = 189
	send(0x11, 189);

	// Disable fractional-N
	//send(0x14, 0x0c);

	//// P0 = 7, P1 = 3, en spi ctrl
	//send(0x17, 0x00);
	//send(0x18, 0x1b);
	//send(0x19, 0xa0);



	// P0 = 7, P1 = 3*15, en spi ctrl
	//send(0x17, 0x01);
	//send(0x18, (45-32) << 3 | 0b011);
	//send(0x19, 0xa0);


	// OUT1 = Complementary CMOS
	send(0x32, 0x81);
	//send(0x32, 0x41);

	// OUT2 = Complementary CMOS
	send(0x34, 0x81);

	uint32_t f = 160000000;

//	for (;;) {

		ad9552_frequency(f);

		// Strobe update
		ad9552_vco_band_auto();

	//	uint32_t kk;
//		for (kk = 0; kk < 10000; kk++);

	//	f += 20000;
//	}
}
#endif


void ad9552_vco_band_auto() {
	send(0x0E, 0x70 | (1 << 2));
	send(0x05, 0x01);
	send(0x0E, 0x70 | (1 << 2));
	send(0x0E, 0x70 | (1 << 2) | (1 << 7));
	send(0x05, 0x01);

}

void ad9552_vco_band_manual(uint8_t band) {

	send(0x10, band << 1);
	send(0x0E, 0x71 | (1 << 2));
	send(0x05, 0x01);
	send(0x0E, 0x71 | (1 << 2));
	send(0x0E, 0x71 | (1 << 2) | (1 << 7));
	send(0x05, 0x01);

}

void ad9552_frequency(uint32_t fOUT) {
  // uint32_t < 4294M
  const uint32_t fVCOmax = 4050000000;
  const uint32_t fVCOmin = 3350000000;
  const uint32_t fVCOmid = (fVCOmin / 2) + (fVCOmax / 2);

  // fOUT < 500M
  uint16_t ODF = 0;
  uint16_t P0 = 0;
  uint16_t P1 = 0;
  int32_t quality = 0;
  unsigned int nODF;
  for (nODF = fVCOmin / fOUT; nODF <= fVCOmax / fOUT; nODF++) {
    int32_t delta = fVCOmid - (nODF * fOUT);
    if (delta < 0) delta = -delta;
    int32_t nquality = (fVCOmax - fVCOmid) - delta;

    uint16_t nP0;
    for (nP0 = 4; nP0 <= 11; nP0++) {
      if ((nODF % nP0) == 0) {

        //printf("ODF  = %llu (%d * %d)\r\n", (unsigned long long)nODF, (int)nP0, (int)(nODF / nP0));
        //printf("qual = %lld\r\n", (long long)nquality);
        //printf("fVCO = %llu\r\n\n", (unsigned long long)(nODF * fOUT));

        if (nquality > quality) {
          quality = nquality;
          ODF = nODF;
          P0 = nP0;
          P1 = nODF / nP0;
        }
        break;
      }
    }

  }
  const uint32_t fVCO = ODF * fOUT;
  const uint32_t fREF = 20000000;

  // fVCO / 20M = N + FRAC/MOD
  // if MOD = 2^19
  // fVCO / 20M = N + FRAC/(2^19)
  // fVCO = N * 20M + FRAC*20M/(2^19)
  // N = fVCO / 20M
  // fVCO % 20M = FRAC*20M/(2^19)
  // FRAC = (fVCO % 20M) / 20M. * (2^19)
  //     ~= ((fVCO % 20M) * (2^19) + 10M) / 20M

  const uint32_t N = fVCO / fREF;
  const uint32_t FRAC = ((uint64_t)(fVCO % fREF) * (uint64_t)(1 << 19) + (uint64_t)(fREF/2)) / (uint64_t)fREF;

  /*printf("fOUT = %llu\r\n", (unsigned long long)fOUT);
  printf("ODF  = %llu (%d * %d)\r\n", (unsigned long long)ODF, (int)P0, (int)P1);
  printf("qual = %lld\r\n", (long long)quality);
  printf("fVCO = %llu\r\n", (unsigned long long)fVCO);
  printf("N    = %llu\r\n", (unsigned long long)N);
  printf("FRAC = %llu\r\n\n", (unsigned long long)FRAC);*/

  //send(0x14, 0x0c);
  //send(0x17, 0x00);
  //send(0x18, 0x1b);
  //send(0x19, 0xa0);


  send(0x11, N);
  send(0x14, 0x08 | ((FRAC == 0) ? 0x04 : 0));
  if (FRAC) {
    send (0x15, FRAC >> 12);
    send (0x16, FRAC >> 4);
  }
  send(0x17, (FRAC << 4) | (P1 >> 5));
  send(0x18, (P1 << 3) | (P0-4));
  send(0x19, 0xa0);
}

uint8_t ad9552_is_locked() {
	return (LPC_GPIO->PIN[0] & (1 << PIN_LOCKED)) ? 1 : 0;
}
