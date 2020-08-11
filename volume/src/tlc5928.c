/*
 * tlc5928.c
 *
 *  Created on: 28 May 2017
 *      Author: Hueburtsonly
 */

#include "tlc5928.h"

#include "ambient.h"
#include "chip.h"
#include "encoder.h"
#include "led.h"
#include "touch.h"
#include "usb.h"


#define LAT 11
#define SCL 12
#define SI0 13
#define SI1 14
#define BLANK 16

volatile uint32_t datums[8];

uint16_t buffer[32][14];


#define DS_LEN ((SUBFRAMES-FRACTIONALS)/2)

// EBR algorithm
// From Dan Gordon, "A derandomization approach to recovering bandlimited signals across a wide range of random sampling rates"
#define S(i) (((i) < k) ? (i) : ((i)+1))
void ebr(uint16_t* a, uint16_t n) {
	if (n == 1) {
		a[0] = 0;
		return;
	}
	uint16_t k = n / 2;

	// Recursive call with half the array size
	ebr(a, k);

	if ((n & 1) == 0) {	// n is even
		// Double the positions of the recursive result
		for (int i = k - 1; i >= 1; i--) {
			a[2 * i] = a[i];
		}
		// Fill the empty space
		for (int i = 1; i < n; i += 2) {
			a[i] = a[i - 1] + k;
		}
	} else { // n is odd
		// Double the positions of the recursive result
		for (int i = k - 1; i >= 1; i--) {
			a[S(2 * i)] = a[i];
		}
		// Fill the empty space
		for (int i = 1; i < n-1; i += 2) {
			a[S(i)] = a[S(i - 1)] + k + 1;
		}
		a[k] = k;
	}
}
#undef S

uint16_t DS_THRESH[DS_LEN];
uint8_t COL_FLIP[SUBFRAMES];
uint16_t FR_PWM[FRACTIONALS]; // ..., PERIOD/4, PERIOD/2, 0xffff

#define PERIOD (200 * 48)

void tlc5928_init() {

	for (int startch = 0; startch < 32; startch += 2) {
		for (int pin = 10; pin < 12; pin++) {
			buffer[startch][pin] = (pin == 11) ? 0xffff: 0;
			buffer[startch+1][pin] = (pin == 10) ? 0xffff: 0;
		}
		buffer[startch][12] = 0; //0xff;
		buffer[startch+1][12] = 0; //0xff;
		buffer[startch][13] = 0xff;
		buffer[startch+1][13] = 0xff;
	}

	ebr(DS_THRESH, DS_LEN);
	for (int i = 0; i < DS_LEN; i++) {
		DS_THRESH[i] = (DS_THRESH[i] + 1) << FRACTIONALS;
	}

	int count = 0;
	for (int i = 0; i < SUBFRAMES; i++) {
		if (COL_FLIP[i] = ((i % 10) == 0) ? 1 : 0) count++;
	}
	if ((count & 1) == 0) {
		COL_FLIP[0] = 0;
	}

	count = PERIOD;
	for (int i = FRACTIONALS-2; i >= 0; i--) {
		count /= 2;
		FR_PWM[i] = count;
	}
	FR_PWM[FRACTIONALS-1] = 0xffff;

    LPC_IOCON->PIO0[LAT] = 0x81;
    LPC_IOCON->PIO0[SCL] = 0x81;
    LPC_IOCON->PIO0[SI0] = 0x81;
    LPC_IOCON->PIO0[SI1] = 0x81;
    LPC_IOCON->PIO0[BLANK] = 0x82; // CT32B1_MAT3


	LPC_GPIO->DIR[0] |= (1 << LAT) | (1 << SCL) | (1 << SI0) | (1 << SI1) | (1 << BLANK);
	LPC_GPIO->SET[0] = (1 << LAT) | (1 << SCL) | (1 << SI0) | (1 << SI1) | (0 << BLANK);

	// Enable interrupt on EP4OUT (LED stuffs)
	LPC_USB->INTEN |= 1 << 8;

	LPC_TIMER32_1->TC = 0;
	LPC_TIMER32_1->PR = 1 - 1;
	LPC_TIMER32_1->MCR = 0b1010; // Reset on MR0 match, Interrupt on MR1 match
	LPC_TIMER32_1->MR[0] = PERIOD - 1;
	LPC_TIMER32_1->MR[1] = PERIOD - 1 - 81;
	LPC_TIMER32_1->MR[3] = 0xffff;
	LPC_TIMER32_1->PWMC = 1 << 3;

	NVIC_EnableIRQ(TIMER_32_1_IRQn);
	NVIC_SetPriority(TIMER_32_1_IRQn, 0);

}

void tlc5928_broadcast(uint16_t state) {
	for (int board = 7; board >= 0; --board) {
		datums[board] = (uint32_t)state | ((uint32_t)state << 16);
	}
	tlc5928_send();
}

static const uint16_t DEMOS[4] = {
	0b0000000000000000,
	0b0000000000000000,
	0b0000000000000000,
	0b0000000000000000
};

void tlc5928_demo(int active) {
	for (int board = 7; board >= 0; --board) {
		uint32_t datum = 0;
		for (int channeloff = 1; channeloff >= 0; channeloff--) {
			int channel = board * 2 + channeloff;
			datum = (datum << 16) | DEMOS[3 & (channel - active)] | ((channel == active) ? 0b1000000000000000 : 0);
		}
		datums[board] = datum;
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

volatile int desiredCsLcd = -1;
volatile int actualCsLcd = -1;
volatile int chosenCsLcd = -1;

void setCsLcd(int desired) {
	desiredCsLcd = desired;
	while (actualCsLcd != desired);
}


uint8_t volatile subframe = 0;
uint8_t colour = 0;

volatile uint32_t tcattime = -1;

void handle_timer_interrupt() {
	LPC_GPIO->CLR[0] = (1 << 23 /* touch sensor */);
	LPC_GPIO->CLR[0] = (1 << BLANK);
	//tcattime = LPC_TIMER32_1->TC;
	LPC_GPIO->SET[0] = (1 << LAT);
	actualCsLcd = chosenCsLcd;

	if (subframe >= SUBFRAMES - FRACTIONALS) {
		// 1, 2
		uint8_t fractionalId = subframe - (SUBFRAMES - FRACTIONALS);
		uint8_t mask = 1 << fractionalId;

		int board;
		int pin;
		LPC_GPIO->CLR[0] = (1 << LAT);
		chosenCsLcd = desiredCsLcd;
		for (board = 3; board >= 0; --board) {
			// LCD Chip select
			LPC_GPIO->CLR[0] = (1 << SCL);
			LPC_GPIO->W[0][SI0] = chosenCsLcd == 2*board;
			LPC_GPIO->W[0][SI1] = chosenCsLcd == 2*board+1;
			LPC_GPIO->SET[0] = (1 << SCL);

			// LCD Reset
			LPC_GPIO->CLR[0] = (1 << SCL);
			LPC_GPIO->W[0][SI0] = 0;
			LPC_GPIO->W[0][SI1] = 0;
			LPC_GPIO->SET[0] = (1 << SCL);

			int indexbase = 4 * board + colour;
			for (pin = 13; pin >= 0; pin--) {
				LPC_GPIO->CLR[0] = (1 << SCL);
				LPC_GPIO->W[0][SI0] = buffer[indexbase][pin] & mask;
				LPC_GPIO->W[0][SI1] = buffer[indexbase + 2][pin] & mask;
				LPC_GPIO->SET[0] = (1 << SCL);
			}
			LPC_GPIO->CLR[0] = (1 << SCL);
		}
		LPC_TIMER32_1->MR[3] = FR_PWM[fractionalId];

	} else if ((subframe & 1) == 0) {

		uint16_t thresh = DS_THRESH[subframe/2];

		int board;
		int pin;
		LPC_GPIO->CLR[0] = (1 << LAT);
		int chosenCsLcd = desiredCsLcd;
		for (board = 3; board >= 0; --board) {
			// LCD Chip select
			LPC_GPIO->CLR[0] = (1 << SCL);
			LPC_GPIO->W[0][SI0] = chosenCsLcd == 2*board;
			LPC_GPIO->W[0][SI1] = chosenCsLcd == 2*board+1;
			LPC_GPIO->SET[0] = (1 << SCL);

			// LCD Reset
			LPC_GPIO->CLR[0] = (1 << SCL);
			LPC_GPIO->W[0][SI0] = 0;
			LPC_GPIO->W[0][SI1] = 0;
			LPC_GPIO->SET[0] = (1 << SCL);

			int indexbase = 4 * board + colour;
			for (pin = 13; pin >= 0; pin--) {
				LPC_GPIO->CLR[0] = (1 << SCL);
				LPC_GPIO->W[0][SI0] = buffer[indexbase][pin] >= thresh;
				LPC_GPIO->W[0][SI1] = buffer[indexbase + 2][pin] >= thresh;
				LPC_GPIO->SET[0] = (1 << SCL);
			}
			LPC_GPIO->CLR[0] = (1 << SCL);
		}
	} else {
		ambient_measure();
		touch_measure();
		encoder_usb_poll();
	}

	subframe = (subframe + 1) % SUBFRAMES;
	//if (subframe == 0)
	colour ^= COL_FLIP[subframe];
}

void TIMER32_1_IRQHandler (void) {
	LPC_TIMER32_1->IR = 2;
	handle_timer_interrupt();
}

#define BRIGHTSCALE (DS_LEN << FRACTIONALS)

static inline uint16_t gamma2(uint8_t x) {
	return ((uint32_t)((uint16_t)x * (uint16_t)x) * (uint32_t)BRIGHTSCALE + 0x8000) >> 16;
}

void handleInterruptOnEp4Out() {
	if (!(EPLIST[EP4OUT] & (1 << 31))) {

		for (int offset = 0; offset < 52; offset += 26) {

			uint8_t startch = EPBUFFER(EP4OUT)[offset] * 2;

			for (int led = 0; led < 12; led++) {
				int pin = led;
				if (led >= 10) pin += 2;
				buffer[startch][pin] = gamma2(EPBUFFER(EP4OUT)[offset + 2 + led]);
				buffer[startch+1][pin] = gamma2(EPBUFFER(EP4OUT)[offset + 2 + 12 + led]);
			}
		}

		activateEndpoint(EP4OUT, 52);
	}
}

void USB_IRQHandler(void) {
	if (LPC_USB->INTSTAT & (1 << 8)) {
		// EP4OUT
		LPC_USB->INTSTAT = 1 << 8;

		handleInterruptOnEp4Out();

	}
}
