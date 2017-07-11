/*
 * encoder.c
 *
 *  Created on: 4 Jun 2017
 *      Author: Hueburtsonly
 */

#include "ambient.h"
#include "cdc.h"
#include "chip.h"
#include "led.h"
#include "touch.h"
#include "usb.h"

#define HSCL 8
#define HPL 9
#define HSO 19 // PORT 1

#define MAX_CHANNELS 16

void encoder_init() {
	LPC_IOCON->PIO1[HSO] = 0x80; // Disable pull-up/down.

	LPC_GPIO->DIR[0] |= (1 << HSCL) | (1 << HPL);
	LPC_GPIO->SET[0] = (1 << HSCL) | (1 << HPL);
}


uint8_t encstate[MAX_CHANNELS];
int16_t enccount[MAX_CHANNELS];

uint8_t btnstate[MAX_CHANNELS];
uint16_t btncount[MAX_CHANNELS];

int8_t channel_count = -1;

const int8_t STATE_TABLE[16] = {
	0, +1, -1, -0x80,
	-1, 0, -0x80, +1,
	+1, -0x80, 0, -1,
	-0x80, -1, +1, 0
};


void encoder_scan() {
	LPC_GPIO->CLR[0] = (1 << HPL);
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	LPC_GPIO->SET[0] = (1 << HPL);
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	//cdc_write_str("Detecting...\r\n");

	for (int base = 0; base < MAX_CHANNELS; base += 2) {

		uint8_t v = 0;

		for (int bit = 0; bit < 8; bit++) {
			__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
			LPC_GPIO->CLR[0] = (1 << HSCL);
			__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
			v <<= 1;
			v |= (1 & LPC_GPIO->B[1][HSO]);
			__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
			LPC_GPIO->SET[0] = (1 << HSCL);
			__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		}
		LPC_GPIO->CLR[0] = (1 << HSCL);
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		// 0b{ det1 s1 b1 a1 det0 s0 b0 a0 }

		//cdc_write_str("  ");

		//cdc_write_int(v);

		//cdc_write_str("\r\n");

		for (int off = 0; off < 2; off++) {
			int ch = base + off;

			// Encoder
			uint8_t newencstate = v & 0x3;
			int8_t delta = STATE_TABLE[(encstate[ch] << 2) | newencstate];
			if (delta == -0x80) {
				if (channel_count != -1) {
					// illegal transition
					led_on(LED_RED);
				}
			} else {
				enccount[ch] += delta;
			}
			encstate[ch] = newencstate;

			// Switch
			if (btnstate[ch] > 0) {
				--btnstate[ch];
			} else if ((btncount[ch] & 0x1) == ((v >> 2) & 0x1)) {
				++btncount[ch];
				btnstate[ch] = 50;
			}

			// Next channel detect
			if (v & 0x8) {
				// This is the last channel
				channel_count = ch + 1;
				return;
			}

			v >>= 4;
		}
	}

	channel_count = -1;
}

void encoder_cdc_demo() {
	//cdc_write_int(result);
	//cdc_write_str("   ");
	for (;;) {
		for (int rpt = 0; rpt < 1000; rpt++)
		encoder_scan();

		cdc_write_int(channel_count);
		for (int ch=0; ch < channel_count; ch++) {
			cdc_write_str("\r\n    ");
			cdc_write_int(enccount[ch]);
			cdc_write_str("    ");
			cdc_write_int(btncount[ch]);
		}
		cdc_write_str("\r\n\r\n");

	}
}


int16_t usbenccount[MAX_CHANNELS];
uint16_t usbbtncount[MAX_CHANNELS];

void encoder_usb_poll() {
	// Protocol: 38-byte packet
	// 0x00: Number channels
	// 0x01: reserved
	// 0x02: Touch
	// 0x03: ^
	// 0x04: Ambient
	// 0x05: ^
	// 0x06: Encoder 0 (delta quarters)
	// 0x07: Button 0 (bit 0 = down, bit 1 = toggle state)
	// 0x08: Encoder 1
	// ...
	// 0x24: Encoder 15
	// 0x25: Encoder 16
	encoder_scan();

	if (EPLIST[EP3IN] & (1 << 31)) {
		// Previous packet hasn't been collected yet
		return;
	}

	(EPBUFFER(EP3IN))[0] = channel_count;
	(EPBUFFER(EP3IN))[2] = touch_result & 0xff;
	(EPBUFFER(EP3IN))[3] = (touch_result >> 8) & 0xff;
	(EPBUFFER(EP3IN))[4] = ambient_result & 0xff;
	(EPBUFFER(EP3IN))[5] = (ambient_result >> 8) & 0xff;

	for (int ch = 0; ch < channel_count; ch++) {

		int16_t delta = (enccount[ch] / 4) - usbenccount[ch];
		if (delta > 127) delta = 127;
		if (delta < -127) delta = -127;
		usbenccount[ch] += delta;

		(EPBUFFER(EP3IN))[6 + ch * 2] = delta;
		(EPBUFFER(EP3IN))[6 + ch * 2 + 1] = btncount[ch];
	}


	activateEndpoint(EP3IN, 38);


//	sendToEndpoint(EP1IN, data, length);
}
