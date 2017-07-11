/*
 * tlc5928.c
 *
 *  Created on: 28 May 2017
 *      Author: Hueburtsonly
 */

#include "chip.h"
#include "led.h"
#include "usb.h"


#define LAT 11
#define SCL 12
#define SI0 13
#define SI1 14
#define BLANK 16

volatile uint32_t datums[8];

uint8_t buffer[32][14];

void tlc5928_init() {

	for (int startch = 0; startch < 32; startch += 2) {
		for (int pin = 10; pin < 12; pin++) {
			buffer[startch][pin] = (pin == 11) ? 0xff: 0;
			buffer[startch+1][pin] = (pin == 10) ? 0xff: 0;
		}
		buffer[startch][13] = 0xff;
		buffer[startch+1][13] = 0xff;
	}

    LPC_IOCON->PIO0[LAT] = 0x81;
    LPC_IOCON->PIO0[SCL] = 0x81;
    LPC_IOCON->PIO0[SI0] = 0x81;
    LPC_IOCON->PIO0[SI1] = 0x81;

	LPC_GPIO->DIR[0] |= (1 << LAT) | (1 << SCL) | (1 << SI0) | (1 << SI1) | (1 << BLANK);
	LPC_GPIO->SET[0] = (1 << LAT) | (1 << SCL) | (1 << SI0) | (1 << SI1) | (0 << BLANK);

	// Enable interrupt on EP4OUT (LED stuffs)
	LPC_USB->INTEN |= 1 << 8;
}

void tlc5928_broadcast(uint16_t state) {
	for (int board = 7; board >= 0; --board) {
		datums[board] = (uint32_t)state | ((uint32_t)state << 16);
	}
	tlc5928_send();
}

static const uint16_t DEMOS[4] = {
	0b0001011010101010,
	0b0001010101010101,
	0b0001101010101010,
	0b0001100101010101
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

void setCsLcd(int desired) {
	desiredCsLcd = desired;
	while (actualCsLcd != desired);
}


uint8_t frame = 0;

void tlc5928_send_from_buffer() {
	//

	// 3.17676 kHz with 16 ch, 16 pins single file
	// 4.32870 kHz with 16 ch, 16 pins double file from uint16_t pair
	// 5.61152 kHz with 16 ch, 16 pins double file from uint32_t
	int board;
	int pin;
	LPC_GPIO->CLR[0] = (1 << LAT);
	int chosenCsLcd = desiredCsLcd;
	for (board = 7; board >= 0; --board) {
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


		//char* boardbase = &((EPBUFFER(EP4OUT))[48 * board + 12 * frame + 2]);
		int indexbase = 4 * board + frame;
		//uint32_t v = datums[board];
		//LPC_GPIO->CLR[0] = (1 << );
		for (pin = 13; pin >= 0; pin--) {

			uint8_t v0 = 0;
			uint8_t v1 = 0;
/*
			if (pin >= 10) {
				if (pin < 14) {
					if (pin < 12) {
						// red/green ctrl
						if (pin == 11 - frame) {
							v0 = 255;
							v1 = 255;
						}
					} else {
						// LCD backlight
						if (pin == 12) {
							v0 = 255;
							v1 = 255;
						}
					}
				} else {
					// CS/RST
					// noop
				}
			} else {
				v0 = boardbase[pin];
				v1 = boardbase[pin + 24];
			}*/

			if (pin < 14) {
				v0 = buffer[indexbase][pin];
				v1 = buffer[indexbase + 2][pin];
			}

			LPC_GPIO->CLR[0] = (1 << SCL);
			LPC_GPIO->W[0][SI0] = v0 >= 128;
			LPC_GPIO->W[0][SI1] = v1 >= 128;
			//v <<= 1;
			LPC_GPIO->SET[0] = (1 << SCL);
		}
		LPC_GPIO->CLR[0] = (1 << SCL);
	}
	LPC_GPIO->SET[0] = (1 << LAT);
	frame ^= 1;
	LPC_GPIO->CLR[0] = (1 << LAT);
	actualCsLcd = chosenCsLcd;
}

void handleInterruptOnEp4Out() {
	if (!(EPLIST[EP4OUT] & (1 << 31))) {

		uint8_t startch = EPBUFFER(EP4OUT)[0] * 2;


		for (int led = 0; led < 12; led++) {
			int pin = led;
			if (led >= 10) pin += 2;
			buffer[startch][pin] = EPBUFFER(EP4OUT)[2 + led];
			buffer[startch+1][pin] = EPBUFFER(EP4OUT)[2 + 12 + led];
			buffer[startch+2][pin] = EPBUFFER(EP4OUT)[2 + 24 + led];
			buffer[startch+3][pin] = EPBUFFER(EP4OUT)[2 + 36 + led];
		}

		activateEndpoint(EP4OUT, 50);
	}
}

void USB_IRQHandler(void) {
	if (LPC_USB->INTSTAT & (1 << 8)) {
		// EP4OUT
		LPC_USB->INTSTAT = 1 << 8;

		handleInterruptOnEp4Out();

	}
}
